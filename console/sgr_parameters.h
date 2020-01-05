// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_SGR_PARAMETERS_H_
#define CONSOLE_SGR_PARAMETERS_H_

#include <stdint.h>

#include <string>

#include "console/export.h"

namespace console {

#define SGR_PARAMETERS_LIST(name, code) \
  constexpr const char* k##name = "\e[" #code "m";
#include "console/sgr_parameters_list.h"
#undef SGR_PARAMETERS_LIST

// Grayscale |level| is 0 to 23.
CONSOLE_EXPORT std::string Grayscale8(uint8_t level = 0);
CONSOLE_EXPORT std::string BgGrayscale8(uint8_t level = 0);

// Rgb8 and BgRgb8 gets color using Ansi8BitColor method.
CONSOLE_EXPORT std::string Rgb8(uint8_t r, uint8_t g, uint8_t b);
CONSOLE_EXPORT std::string BgRgb8(uint8_t r, uint8_t g, uint8_t b);

CONSOLE_EXPORT std::string Rgb24(uint8_t r, uint8_t g, uint8_t b);
CONSOLE_EXPORT std::string BgRgb24(uint8_t r, uint8_t g, uint8_t b);

// Returns Ansi 8 bit color using the equation below.
// 6 × 6 × 6 cube (216 colors): 16 + 36 × r + 6 × g + b (0 ≤ r, g, b ≤ 5)
CONSOLE_EXPORT uint8_t Ansi8BitColor(uint8_t r, uint8_t g, uint8_t b);

}  // namespace console

#endif  // CONSOLE_SGR_PARAMETERS_H_