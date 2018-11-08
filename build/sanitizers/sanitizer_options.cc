// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file contains the default options for various compiler-based dynamic
// tools.

#if defined(ADDRESS_SANITIZER) && defined(__MACH__)
#include <crt_externs.h>  // for _NSGetArgc, _NSGetArgv
#include <string.h>
#endif  // ADDRESS_SANITIZER && OS_MACOSX

#if defined(ADDRESS_SANITIZER) || defined(LEAK_SANITIZER) ||  \
    defined(MEMORY_SANITIZER) || defined(THREAD_SANITIZER) || \
    defined(UNDEFINED_SANITIZER)
// Functions returning default options are declared weak in the tools' runtime
// libraries. To make the linker pick the strong replacements for those
// functions from this module, we explicitly force its inclusion by passing
// -Wl,-u_sanitizer_options_link_helper
extern "C"
void _sanitizer_options_link_helper() { }

// The callbacks we define here will be called from the sanitizer runtime, but
// aren't referenced from the Chrome executable. We must ensure that those
// callbacks are not sanitizer-instrumented, and that they aren't stripped by
// the linker.
#define SANITIZER_HOOK_ATTRIBUTE                                           \
  extern "C"                                                               \
  __attribute__((no_sanitize("address", "memory", "thread", "undefined"))) \
  __attribute__((visibility("default")))                                   \
  __attribute__((used))
#endif

#if defined(ADDRESS_SANITIZER)
// Default options for AddressSanitizer in various configurations:
//   malloc_context_size=5 - limit the size of stack traces collected by ASan
//     for each malloc/free by 5 frames. These stack traces tend to accumulate
//     very fast in applications using JIT (v8 in Chrome's case), see
//     https://code.google.com/p/address-sanitizer/issues/detail?id=177
//   symbolize=1 - enable in-process symbolization.
//   legacy_pthread_cond=1 - run in the libpthread 2.2.5 compatibility mode to
//     work around libGL.so using the obsolete API, see
//     http://crbug.com/341805. This may break if pthread_cond_t objects are
//     accessed by both instrumented and non-instrumented binaries (e.g. if
//     they reside in shared memory). This option is going to be deprecated in
//     upstream AddressSanitizer and must not be used anywhere except the
//     official builds.
//   check_printf=1 - check the memory accesses to printf (and other formatted
//     output routines) arguments.
//   use_sigaltstack=1 - handle signals on an alternate signal stack. Useful
//     for stack overflow detection.
//   strip_path_prefix=/../../ - prefixes up to and including this
//     substring will be stripped from source file paths in symbolized reports
//   fast_unwind_on_fatal=1 - use the fast (frame-pointer-based) stack unwinder
//     to print error reports. V8 doesn't generate debug info for the JIT code,
//     so the slow unwinder may not work properly.
//   detect_stack_use_after_return=1 - use fake stack to delay the reuse of
//     stack allocations and detect stack-use-after-return errors.
#if defined(__linux__)
// Default AddressSanitizer options for buildbots and non-official builds.
const char *kAsanDefaultOptions =
    "symbolize=1 check_printf=1 use_sigaltstack=1 "
    "detect_leaks=0 strip_path_prefix=/../../ fast_unwind_on_fatal=1 "
    "detect_stack_use_after_return=1 ";

#elif defined(__MACH__)
const char *kAsanDefaultOptions =
    "check_printf=1 use_sigaltstack=1 "
    "strip_path_prefix=/../../ fast_unwind_on_fatal=1 "
    "detect_stack_use_after_return=1 detect_odr_violation=0 ";
static const char kNaClDefaultOptions[] = "handle_segv=0";
static const char kNaClFlag[] = "--type=nacl-loader";
#endif  // __linux__

#if defined(__linux__) || defined(__MACH__)
SANITIZER_HOOK_ATTRIBUTE const char *__asan_default_options() {
#if defined(__MACH__)
  char*** argvp = _NSGetArgv();
  int* argcp = _NSGetArgc();
  if (!argvp || !argcp) return kAsanDefaultOptions;
  char** argv = *argvp;
  int argc = *argcp;
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], kNaClFlag) == 0) {
      return kNaClDefaultOptions;
    }
  }
#endif  // __MACH__
  return kAsanDefaultOptions;
}

extern "C" char kASanDefaultSuppressions[];

SANITIZER_HOOK_ATTRIBUTE const char *__asan_default_suppressions() {
  return kASanDefaultSuppressions;
}
#endif  // __linux__ || __MACH__
#endif  // ADDRESS_SANITIZER

#if defined(THREAD_SANITIZER) && defined(__linux__)
// Default options for ThreadSanitizer in various configurations:
//   detect_deadlocks=1 - enable deadlock (lock inversion) detection.
//   second_deadlock_stack=1 - more verbose deadlock reports.
//   report_signal_unsafe=0 - do not report async-signal-unsafe functions
//     called from signal handlers.
//   report_thread_leaks=0 - do not report unjoined threads at the end of
//     the program execution.
//   print_suppressions=1 - print the list of matched suppressions.
//   history_size=7 - make the history buffer proportional to 2^7 (the maximum
//     value) to keep more stack traces.
//   strip_path_prefix=/../../ - prefixes up to and including this
//     substring will be stripped from source file paths in symbolized reports.
const char kTsanDefaultOptions[] =
    "detect_deadlocks=1 second_deadlock_stack=1 report_signal_unsafe=0 "
    "report_thread_leaks=0 print_suppressions=1 history_size=7 "
    "strict_memcmp=0 strip_path_prefix=/../../ ";

SANITIZER_HOOK_ATTRIBUTE const char *__tsan_default_options() {
  return kTsanDefaultOptions;
}

extern "C" char kTSanDefaultSuppressions[];

SANITIZER_HOOK_ATTRIBUTE const char *__tsan_default_suppressions() {
  return kTSanDefaultSuppressions;
}

#endif  // THREAD_SANITIZER && __linux__

#if defined(MEMORY_SANITIZER)
// Default options for MemorySanitizer:
//   intercept_memcmp=0 - do not detect uninitialized memory in memcmp() calls.
//     Pending cleanup, see http://crbug.com/523428
//   strip_path_prefix=/../../ - prefixes up to and including this
//     substring will be stripped from source file paths in symbolized reports.
const char kMsanDefaultOptions[] =
    "intercept_memcmp=0 strip_path_prefix=/../../ ";

SANITIZER_HOOK_ATTRIBUTE const char *__msan_default_options() {
  return kMsanDefaultOptions;
}

#endif  // MEMORY_SANITIZER

#if defined(LEAK_SANITIZER)
// Default options for LeakSanitizer:
//   print_suppressions=1 - print the list of matched suppressions.
//   strip_path_prefix=/../../ - prefixes up to and including this
//     substring will be stripped from source file paths in symbolized reports.
const char kLsanDefaultOptions[] =
    "print_suppressions=1 strip_path_prefix=/../../ ";

SANITIZER_HOOK_ATTRIBUTE const char *__lsan_default_options() {
  return kLsanDefaultOptions;
}

extern "C" char kLSanDefaultSuppressions[];

SANITIZER_HOOK_ATTRIBUTE const char *__lsan_default_suppressions() {
  return kLSanDefaultSuppressions;
}

#endif  // LEAK_SANITIZER

#if defined(UNDEFINED_SANITIZER)
// Default options for UndefinedBehaviorSanitizer:
//   print_stacktrace=1 - print the stacktrace when UBSan reports an error.
const char kUbsanDefaultOptions[] =
    "print_stacktrace=1 strip_path_prefix=/../../ ";

SANITIZER_HOOK_ATTRIBUTE const char* __ubsan_default_options() {
  return kUbsanDefaultOptions;
}

#endif  // UNDEFINED_SANITIZER