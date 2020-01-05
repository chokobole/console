// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/sgr_parameters.h"

#include "absl/strings/str_cat.h"

namespace console {

std::string Grayscale8(uint8_t level) {
  if (level > 24) return "";
  return absl::StrCat("\e[38;5;", level + 232, "m");
}

std::string BgGrayscale8(uint8_t level) {
  if (level > 24) return "";
  return absl::StrCat("\e[48;5;", level + 232, "m");
}

std::string Rgb8(uint8_t r, uint8_t g, uint8_t b) {
  return absl::StrCat("\e[38;5;", Ansi8BitColor(r, g, b), "m");
}

std::string BgRgb8(uint8_t r, uint8_t g, uint8_t b) {
  return absl::StrCat("\e[48;5;", Ansi8BitColor(r, g, b), "m");
}

std::string Rgb24(uint8_t r, uint8_t g, uint8_t b) {
  return absl::StrCat("\e[38;2;", r, ";", g, ";", b, "m");
}

std::string BgRgb24(uint8_t r, uint8_t g, uint8_t b) {
  return absl::StrCat("\e[48;2;", r, ";", g, ";", b, "m");
}

uint8_t Ansi8BitColor(uint8_t r, uint8_t g, uint8_t b) {
  double r_scaled = r / 255.0 * 5;
  double g_scaled = g / 255.0 * 5;
  double b_scaled = b / 255.0 * 5;
  return 16 + 36 * r_scaled + 6 * g_scaled + b_scaled;
}

}  // namespace console