// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include <ostream>

#include "console/export.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define OS_WIN
#endif

namespace console {

class CONSOLE_EXPORT Console {
 public:
  struct Info {
    bool support_ansi = false;
    bool support_8bit_color = false;
    bool support_truecolor = false;
  };

#if defined(OS_WIN)
  static bool EnableAnsi(std::ostream& os);
#endif
  static Info GetInfo();
  static bool IsConnected(std::ostream& os);
};

}  // namespace console

#endif  // CONSOLE_CONSOLE_H_