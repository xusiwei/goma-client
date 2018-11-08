vars = {
     "chromium_git": "https://chromium.googlesource.com",
}

deps = {
     # protobuf 3.6.1
     # Note: When you update protobuf, you will need to update
     # test/goma_data.pb.{h,cc}. Copying them from your output directory should
     # work.
     "client/third_party/protobuf/protobuf":
     "https://github.com/google/protobuf.git@48cb18e5c419ddd23d9badcfe4e9df7bde1979b2",

     # google-glog
     "client/third_party/glog":
     "https://github.com/google/glog.git@2063b387080c1e7adffd33ca07adff0eb346ff1a",

     # googletest
     "client/third_party/gtest":
     Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' +
         '145d05750b15324899473340c8dd5af50d125d33',

     # zlib 1.2.8
     "client/third_party/zlib":
     "https://goma.googlesource.com/zlib.git@50893291621658f355bc5b4d450a8d06a563053d",

     # xz v5.2.0
     "client/third_party/xz":
     "https://goma.googlesource.com/xz.git@fbafe6dd0892b04fdef601580f2c5b0e3745655b",

     # jsoncpp
     "client/third_party/jsoncpp/source":
     Var("chromium_git") + '/external/github.com/open-source-parsers/jsoncpp.git@f572e8e42e22cfcf5ab0aea26574f408943edfa4', # from svn 248

     # chrome's tools/clang
     "client/tools/clang":
     "https://chromium.googlesource.com/chromium/src/tools/clang.git",

     # chrome's deps/third_party/boringssl
     "client/third_party/boringssl/src":
     "https://boringssl.googlesource.com/boringssl@d2ed382e647bfe6148bdc192d77b48406af85a94",

     # google-breakpad
     "client/third_party/breakpad/breakpad":
     Var("chromium_git") + "/breakpad/breakpad.git@" +
         "54fa71efbe50fb2b58096d871575b59e12edba6d",

     # lss
     "client/third_party/lss":
     Var("chromium_git") + "/linux-syscall-support.git@" +
         "a89bf7903f3169e6bc7b8efc10a73a7571de21cf",

     # chrome's patched-yasm
     "client/third_party/yasm/source/patched-yasm":
     Var("chromium_git") + "/chromium/deps/yasm/patched-yasm.git@" +
         "b98114e18d8b9b84586b10d24353ab8616d4c5fc",

     # libc++ r323563
     "client/third_party/libc++/trunk":
     Var("chromium_git") + "/chromium/llvm-project/libcxx.git@" +
         "27c341db41bc9df5c6f19cde65f002d6f1c2eb3c",

     # libc++abi r323495
     "client/third_party/libc++abi/trunk":
     Var("chromium_git") + "/chromium/llvm-project/libcxxabi.git@" +
         "e1601db2504857d44db88a5d4e2ca50b32bbb7d9",

     # libFuzzer
     "client/third_party/libFuzzer/src":
     Var("chromium_git") + "/chromium/llvm-project/llvm/lib/Fuzzer.git@" +
         "9aa0bddeb6820f6e5d897da410e1e8a3f7fd4b8e",

     # abseil
     "client/third_party/abseil/src":
     "https://github.com/abseil/abseil-cpp.git@070f6e47b33a2909d039e620c873204f78809492",

     # google benchmark v1.4.1
     "client/third_party/benchmark/src":
     "https://github.com/google/benchmark.git@e776aa0275e293707b6a0901e0e8d8a8a3679508",

     # clang format scripts
     "client/buildtools/clang_format/script":
     Var("chromium_git") + "/chromium/llvm-project/cfe/tools/" +
     "clang-format.git@0653eee0c81ea04715c635dd0885e8096ff6ba6d",

     # Jinja2 template engine v2.10
     "client/third_party/jinja2":
     "https://github.com/pallets/jinja.git@78d2f672149e5b9b7d539c575d2c1bfc12db67a9",

     # Markupsafe module v1.0
     "client/third_party/markupsafe":
     "https://github.com/pallets/markupsafe.git@d2a40c41dd1930345628ea9412d97e159f828157",

     # depot_tools
     'client/third_party/depot_tools':
     Var('chromium_git') + '/chromium/tools/depot_tools.git',

     # gflags 2.2.1
     "client/third_party/gflags/src":
     "https://github.com/gflags/gflags.git@46f73f88b18aee341538c0dfc22b1710a6abedef",
}

hooks = [
     {
       "name": "clang",
       "pattern": ".",
       "action": ["python", "client/tools/clang/scripts/update.py"],
     },

     # Pull binutils for linux, it is used for simpletry test.
     {
       "name": "binutils",
       "pattern": ".",
       "action": [
         "python",
         "client/test/third_party/binutils/download.py",
       ],
     },

     # Pull GN binaries.
     {
       "name": "gn_win",
       "pattern": ".",
       "action": [ "download_from_google_storage",
                   "--no_resume",
                   "--platform=win32",
                   "--no_auth",
                   "--bucket", "chromium-gn",
                   "-s", "client/buildtools/win/gn.exe.sha1",
       ],
     },
     {
       "name": "gn_mac",
       "pattern": ".",
       "action": [ "download_from_google_storage",
                   "--no_resume",
                   "--platform=darwin",
                   "--no_auth",
                   "--bucket", "chromium-gn",
                   "-s", "client/buildtools/mac/gn.sha1",
       ],
     },
     {
       "name": "gn_linux64",
       "pattern": ".",
       "action": [ "download_from_google_storage",
                   "--no_resume",
                   "--platform=linux*",
                   "--no_auth",
                   "--bucket", "chromium-gn",
                   "-s", "client/buildtools/linux64/gn.sha1",
       ],
     },
     # Pull clang-format binaries using checked-in hashes.
     {
         'name': 'clang_format_win',
         'pattern': '.',
         'condition': 'host_os == "win"',
         'action': [ 'download_from_google_storage',
                     '--no_resume',
                     '--no_auth',
                     '--bucket', 'chromium-clang-format',
                     '-s', 'client/buildtools/win/clang-format.exe.sha1',
         ],
     },
     {
         'name': 'clang_format_mac',
         'pattern': '.',
         'condition': 'host_os == "mac"',
         'action': [ 'download_from_google_storage',
                     '--no_resume',
                     '--no_auth',
                     '--bucket', 'chromium-clang-format',
                     '-s', 'client/buildtools/mac/clang-format.sha1',
         ],
     },
     {
         'name': 'clang_format_linux',
         'pattern': '.',
         'condition': 'host_os == "linux"',
         'action': [ 'download_from_google_storage',
                     '--no_resume',
                     '--no_auth',
                     '--bucket', 'chromium-clang-format',
                     '-s', 'client/buildtools/linux64/clang-format.sha1',
         ],
     },
     # Update the Windows toolchain if necessary.
     {
       'name': 'win_toolchain',
       'pattern': '.',
       'action': ['python', 'client/build/vs_toolchain.py', 'update'],
     },
     # Update the Mac toolchain if necessary.
     {
       'name': 'mac_toolchain',
       'pattern': '.',
       'condition': 'checkout_ios or checkout_mac',
       'action': ['python', 'client/build/mac_toolchain.py'],
     },

     # Ensure that the DEPS'd "depot_tools" has its self-update capability
     # disabled.
     {
       'name': 'disable_depot_tools_selfupdate',
       'pattern': '.',
       'action': [
         'python',
         'client/third_party/depot_tools/update_depot_tools_toggle.py',
         '--disable',
       ],
     },
]