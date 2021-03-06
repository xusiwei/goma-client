// Copyright 2018 The Goma Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "compiler_info_builder.h"

#include "basictypes.h"
#include "compiler_flags_parser.h"
#include "compiler_info.h"
#include "compiler_type_specific_collection.h"
#include "cxx/cxx_compiler_info.h"
#include "gtest/gtest.h"
#include "mypath.h"
#include "path.h"
#include "subprocess.h"
#include "unittest_util.h"
#include "util.h"

namespace devtools_goma {

class CompilerInfoBuilderTest : public testing::Test {
 protected:
  void SetUp() override { CheckTempDirectory(GetGomaTmpDir()); }

  void AppendPredefinedMacros(const std::string& macro, CompilerInfoData* cid) {
    cid->mutable_cxx()->set_predefined_macros(cid->cxx().predefined_macros() +
                                              macro);
  }

  std::string TestDir() {
    // This module is in out\Release.
    const std::string parent_dir = file::JoinPath(GetMyDirectory(), "..");
    const std::string top_dir = file::JoinPath(parent_dir, "..");
    return file::JoinPath(top_dir, "test");
  }

  CompilerTypeSpecificCollection cts_collection_;
};

TEST_F(CompilerInfoBuilderTest, DependsOnCwd) {
  {
    std::unique_ptr<CompilerInfoData> cid(new CompilerInfoData);
    cid->mutable_cxx()->add_cxx_system_include_paths("/usr/local/include");
    cid->mutable_cxx()->add_cxx_system_include_paths(
        "/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include");
    cid->mutable_cxx()->add_cxx_system_include_paths(
        "/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include-fixed");
    cid->mutable_cxx()->add_cxx_system_include_paths("/usr/include");
    cid->set_found(true);
    CxxCompilerInfo info(std::move(cid));
    EXPECT_FALSE(info.DependsOnCwd("/tmp"));
    EXPECT_TRUE(info.DependsOnCwd("/usr"));
  }

  {
    std::unique_ptr<CompilerInfoData> cid(new CompilerInfoData);
    cid->mutable_cxx()->add_cxx_system_include_paths("/tmp/.");
    cid->mutable_cxx()->add_cxx_system_include_paths("/usr/local/include");
    cid->mutable_cxx()->add_cxx_system_include_paths(
        "/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include");
    cid->mutable_cxx()->add_cxx_system_include_paths(
        "/usr/lib/gcc/x86_64-linux-gnu/4.4.3/include-fixed");
    cid->mutable_cxx()->add_cxx_system_include_paths("/usr/include");
    cid->set_found(true);
    CxxCompilerInfo info(std::move(cid));
    EXPECT_TRUE(info.DependsOnCwd("/tmp"));
    EXPECT_FALSE(info.DependsOnCwd("/usr/src"));
  }
}

TEST_F(CompilerInfoBuilderTest, FillFromCompilerOutputsShouldUseProperPath) {
  std::vector<std::string> envs;
#ifdef _WIN32
  const std::string clang = file::JoinPath(TestDir(), "clang.bat");
  InstallReadCommandOutputFunc(ReadCommandOutputByRedirector);
  envs.emplace_back("PATHEXT=" + GetEnv("PATHEXT").value_or(""));
#else
  const std::string clang = file::JoinPath(TestDir(), "clang");
  InstallReadCommandOutputFunc(ReadCommandOutputByPopen);
#endif
  std::vector<std::string> args = {
      clang,
  };
  envs.emplace_back("PATH=" + GetEnv("PATH").value_or(""));
  std::unique_ptr<CompilerFlags> flags(CompilerFlagsParser::MustNew(args, "."));
  std::unique_ptr<CompilerInfoData> data(
      cts_collection_.Get(flags->type())
          ->BuildCompilerInfoData(*flags, clang, envs));
  EXPECT_TRUE(data.get());
  EXPECT_EQ(0, data->failed_at());
}

TEST_F(CompilerInfoBuilderTest, DependsOnCwdWithResource) {
  TmpdirUtil tmpdir("is_cwd_relative");
  tmpdir.CreateEmptyFile("asan_blacklist.txt");

  {  // under cwd.
    CompilerInfoData::ResourceInfo r_data;
    CompilerInfoBuilder::ResourceInfoFromPath(
        ".", tmpdir.FullPath("asan_blacklist.txt"),
        CompilerInfoData::CLANG_RESOURCE, &r_data);
    std::unique_ptr<CompilerInfoData> cid(new CompilerInfoData);
    cid->set_found(true);
    cid->mutable_cxx();
    *cid->add_resource() = r_data;

    CxxCompilerInfo info(std::move(cid));
    EXPECT_TRUE(info.DependsOnCwd(tmpdir.tmpdir()));
    EXPECT_FALSE(info.DependsOnCwd("/nonexistent"));
  }

  {  // relative path file.
    CompilerInfoData::ResourceInfo r_data;
    CompilerInfoBuilder::ResourceInfoFromPath(
        tmpdir.tmpdir(), "asan_blacklist.txt", CompilerInfoData::CLANG_RESOURCE,
        &r_data);
    std::unique_ptr<CompilerInfoData> cid(new CompilerInfoData);
    cid->set_found(true);
    cid->mutable_cxx();
    *cid->add_resource() = r_data;

    CxxCompilerInfo info(std::move(cid));
    EXPECT_TRUE(info.DependsOnCwd(tmpdir.tmpdir()));
    EXPECT_TRUE(info.DependsOnCwd("/nonexistent"));
  }
}

#ifdef __linux__
// Checks we can take CompilerInfo from /usr/bin/gcc etc.
TEST_F(CompilerInfoBuilderTest, GccSmoke) {
  InstallReadCommandOutputFunc(ReadCommandOutputByPopen);

  // Assuming testcases[i][0] is a path to gcc.
  const std::vector<std::vector<std::string>> testcases = {
      {
          "/usr/bin/gcc",
      },
      {"/usr/bin/gcc", "-xc"},
      {"/usr/bin/gcc", "-xc++"},
      {
          "/usr/bin/g++",
      },
      {"/usr/bin/g++", "-xc"},
      {"/usr/bin/g++", "-xc++"},
  };
  const std::vector<std::string> envs;

  for (const auto& args : testcases) {
    std::unique_ptr<CompilerFlags> flags(
        CompilerFlagsParser::MustNew(args, "."));
    CxxCompilerInfo compiler_info(
        cts_collection_.Get(flags->type())
            ->BuildCompilerInfoData(*flags, args[0], envs));

    EXPECT_FALSE(compiler_info.HasError());
  }
}
#endif

// Checks we can take CompilerInfo from
// third_party/llvm-build/Release+Assets/bin/clang etc.
TEST_F(CompilerInfoBuilderTest, ClangSmoke) {
  std::string source_root_path = std::string(
      file::Dirname(file::Dirname(devtools_goma::GetMyDirectory())));

#ifdef _WIN32
  InstallReadCommandOutputFunc(ReadCommandOutputByRedirector);
  const std::vector<std::string> envs{
      "PATH=" + GetEnv("PATH").value_or(""),
      "PATHEXT=" + GetEnv("PATHEXT").value_or(""),
  };
#else
  InstallReadCommandOutputFunc(ReadCommandOutputByPopen);
  const std::vector<std::string> envs;
#endif

  std::string clang_path = GetClangPath();
  ASSERT_TRUE(!clang_path.empty());

  const std::vector<std::vector<std::string>> testcases = {
      {clang_path},
      {clang_path, "-xc"},
      {clang_path, "-xc++"},
  };

  for (const auto& args : testcases) {
    std::unique_ptr<CompilerFlags> flags(
        CompilerFlagsParser::MustNew(args, "."));
    CxxCompilerInfo compiler_info(
        cts_collection_.Get(flags->type())
            ->BuildCompilerInfoData(*flags, args[0], envs));

    EXPECT_FALSE(compiler_info.HasError());
  }
}

}  // namespace devtools_goma
