// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/console.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

#include "base/strings/string_util.h"

#if defined(OS_WIN)
#include <io.h>
#include <windows.h>
#include <winver.h>
#else
#include <unistd.h>
#endif

namespace console {

namespace {

static Console::Info kConsoleInfo;
bool kConsoleInfoInitialized = false;

#if defined(OS_WIN)
// Thid was taken and modified from
// LICENSE: undefined
// URL:  https://github.com/agauniyal/rang/blob/master/include/rang.hpp
bool IsMsysPty(int fd) noexcept {
  // Dynamic load for binary compability with old Windows
  const auto ptrGetFileInformationByHandleEx =
      reinterpret_cast<decltype(&GetFileInformationByHandleEx)>(
          GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
                         "GetFileInformationByHandleEx"));
  if (!ptrGetFileInformationByHandleEx) {
    return false;
  }

  HANDLE h = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
  if (h == INVALID_HANDLE_VALUE) {
    return false;
  }

  // Check that it's a pipe:
  if (GetFileType(h) != FILE_TYPE_PIPE) {
    return false;
  }

  // POD type is binary compatible with FILE_NAME_INFO from WinBase.h
  // It have the same alignment and used to avoid UB in caller code
  struct MY_FILE_NAME_INFO {
    DWORD FileNameLength;
    WCHAR FileName[MAX_PATH];
  };

  auto pNameInfo = std::unique_ptr<MY_FILE_NAME_INFO>(new (std::nothrow)
                                                          MY_FILE_NAME_INFO());
  if (!pNameInfo) {
    return false;
  }

  // Check pipe name is template of
  // {"cygwin-","msys-"}XXXXXXXXXXXXXXX-ptyX-XX
  if (!ptrGetFileInformationByHandleEx(h, FileNameInfo, pNameInfo.get(),
                                       sizeof(MY_FILE_NAME_INFO))) {
    return false;
  }
  std::wstring name(pNameInfo->FileName,
                    pNameInfo->FileNameLength / sizeof(WCHAR));
  if ((name.find(L"msys-") == std::wstring::npos &&
       name.find(L"cygwin-") == std::wstring::npos) ||
      name.find(L"-pty") == std::wstring::npos) {
    return false;
  }

  return true;
}

// Thid was taken and modified from
// LICENSE: undefined
// URL:  https://github.com/agauniyal/rang/blob/master/include/rang.hpp
HANDLE GetConsoleHandle(const std::streambuf* osbuf) {
  if (osbuf == std::cout.rdbuf()) {
    static const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    return hStdout;
  } else if (osbuf == std::cerr.rdbuf() || osbuf == std::clog.rdbuf()) {
    static const HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
    return hStderr;
  }
  return INVALID_HANDLE_VALUE;
}

// Thid was taken and modified from
// LICENSE: undefined
// URL:  https://github.com/agauniyal/rang/blob/master/include/rang.hpp
bool SetWinTermAnsi(std::ostream& os) {
  const std::streambuf* osbuf = os.rdbuf();
  HANDLE h = GetConsoleHandle(osbuf);
  if (h == INVALID_HANDLE_VALUE) {
    return false;
  }
  DWORD dwMode = 0;
  if (!GetConsoleMode(h, &dwMode)) {
    return false;
  }
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(h, dwMode)) {
    return false;
  }
  return true;
}

// https://stackoverflow.com/a/47583450
bool GetVersionInfo(DWORD* major_version, DWORD* minor_version,
                    DWORD* buildnum) {
  const wchar_t* system = L"kernel32.dll";
  DWORD dummy;
  DWORD cbInfo =
      GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, system, &dummy);
  std::vector<char> buffer(cbInfo);
  if (!GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, system, dummy, buffer.size(),
                             &buffer[0]))
    return false;
  void* p = nullptr;
  UINT size = 0;
  if (!VerQueryValueW(buffer.data(), L"\\", &p, &size)) return false;
  auto pFixed = static_cast<const VS_FIXEDFILEINFO*>(p);
  *major_version = HIWORD(pFixed->dwFileVersionMS);
  *minor_version = LOWORD(pFixed->dwFileVersionMS);
  *buildnum = HIWORD(pFixed->dwFileVersionLS);
  return true;
}

#endif

}  // namespace

#if defined(OS_WIN)
// static
bool Console::EnableAnsi(std::ostream& os) { return SetWinTermAnsi(os); }
#endif

// static
Console::Info Console::GetInfo() {
  if (kConsoleInfoInitialized) return kConsoleInfo;
  kConsoleInfoInitialized = true;

#if defined(OS_WIN)
  DWORD major_version;
  DWORD minor_version;
  DWORD buildnum;
  if (GetVersionInfo(&major_version, &minor_version, &buildnum)) {
    if (major_version >= 10 && buildnum >= 14931) {
      kConsoleInfo.support_ansi = true;
      kConsoleInfo.support_truecolor = true;
      return kConsoleInfo;
    } else if (major_version >= 10 && buildnum >= 10586) {
      kConsoleInfo.support_ansi = true;
      kConsoleInfo.support_8bit_color = true;
      return kConsoleInfo;
    }
  }
#endif

  static const char* kTerms[] = {
      "ansi",  "bvterm", "color",   "console", "cygwin", "konsole",
      "linux", "putty",  "scoansi", "screen",  "tmux",
  };

  static const char* kTermsPrefix[] = {
      "eterm", "kterm", "vt100", "vt102", "vt220", "vt320", "xterm",
  };

  const char* term_env = std::getenv("TERM");
  if (!term_env) return kConsoleInfo;
  kConsoleInfo.support_ansi =
      std::any_of(std::begin(kTerms), std::end(kTerms),
                  [term_env](const char* term) {
                    return absl::string_view(term_env) == term;
                  }) ||
      std::any_of(std::begin(kTermsPrefix), std::end(kTermsPrefix),
                  [term_env](const char* prefix) {
                    return base::StartsWith(term_env, prefix);
                  });

  const char* colorterm_env = std::getenv("COLORTERM");
  if (colorterm_env) {
    absl::string_view colorterm(colorterm_env);
    if (colorterm == "truecolor") {
      kConsoleInfo.support_truecolor = true;
    } else if (base::EndsWith(colorterm, "-256")) {
      kConsoleInfo.support_8bit_color = true;
    }
  }

  return kConsoleInfo;
};

// Thid was taken and modified from
// LICENSE: undefined
// URL:  https://github.com/agauniyal/rang/blob/master/include/rang.hpp
// static
bool Console::IsConnected(std::ostream& os) {
  std::streambuf* osbuf = os.rdbuf();
#if defined(OS_WIN)
  if (osbuf == std::cout.rdbuf()) {
    static const bool cout_term =
        (_isatty(_fileno(stdout)) || IsMsysPty(_fileno(stdout)));
    return cout_term;
  } else if (osbuf == std::cerr.rdbuf() || osbuf == std::clog.rdbuf()) {
    static const bool cerr_term =
        (_isatty(_fileno(stderr)) || IsMsysPty(_fileno(stderr)));
    return cerr_term;
  }
#else
  if (osbuf == std::cout.rdbuf()) {
    static const bool cout_term = isatty(fileno(stdout)) != 0;
    return cout_term;
  } else if (osbuf == std::cerr.rdbuf() || osbuf == std::clog.rdbuf()) {
    static const bool cerr_term = isatty(fileno(stderr)) != 0;
    return cerr_term;
  }
#endif
  return false;
}

#undef OS_WIN

}  // namespace console