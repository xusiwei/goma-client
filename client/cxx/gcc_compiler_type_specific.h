// Copyright 2018 The Goma Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVTOOLS_GOMA_CLIENT_CXX_GCC_COMPILER_TYPE_SPECIFIC_H_
#define DEVTOOLS_GOMA_CLIENT_CXX_GCC_COMPILER_TYPE_SPECIFIC_H_

#include "cxx/cxx_compiler_type_specific.h"
#include "cxx/gcc_compiler_info_builder.h"

namespace devtools_goma {

class GCCCompilerTypeSpecific : public CxxCompilerTypeSpecific {
 public:
  GCCCompilerTypeSpecific(const GCCCompilerTypeSpecific&) = delete;
  void operator=(const GCCCompilerTypeSpecific&) = delete;

  std::unique_ptr<CompilerInfoData> BuildCompilerInfoData(
      const CompilerFlags& flags,
      const string& local_compiler_path,
      const std::vector<string>& compiler_info_envs) override;

 private:
  GCCCompilerTypeSpecific() = default;

  GCCCompilerInfoBuilder compiler_info_builder_;

  friend class CompilerTypeSpecificCollection;
};

}  // namespace devtools_goma

#endif  // DEVTOOLS_GOMA_CLIENT_CXX_GCC_COMPILER_TYPE_SPECIFIC_H_
