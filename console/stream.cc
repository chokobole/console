// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/stream.h"

#include "color/color_conversion.h"

namespace console {

Stream::Stream(std::ostream& ostream)
    : ostream_(ostream), console_info_(Console::GetInfo()) {}

Stream::~Stream() { Reset(); }

#define SGR_PARAMETERS_LIST(name, code) \
  Stream& Stream::name() {              \
    ostream_ << k##name;                \
    return *this;                       \
  }
#include "console/sgr_parameters_list.h"
#undef SGR_PARAMETERS_LIST

Stream& Stream::Rgb(color::Rgb rgb) {
  return Rgb(rgb.data.r, rgb.data.g, rgb.data.b);
}

Stream& Stream::Rgb(uint8_t r, uint8_t g, uint8_t b) {
  if (console_info_.support_truecolor) {
    ostream_ << Rgb24(r, g, b);
  } else if (console_info_.support_8bit_color) {
    if (r == g && g == b) {
      ostream_ << Grayscale8(r * (23.0 / 255.0));
    } else {
      ostream_ << Rgb8(r, g, b);
    }
  }
  return *this;
}

Stream& Stream::BgRgb(color::Rgb rgb) {
  return BgRgb(rgb.data.r, rgb.data.g, rgb.data.b);
}

Stream& Stream::BgRgb(uint8_t r, uint8_t g, uint8_t b) {
  if (console_info_.support_truecolor) {
    ostream_ << BgRgb24(r, g, b);
  } else if (console_info_.support_8bit_color) {
    if (r == g && g == b) {
      ostream_ << BgGrayscale8(r * (23.0 / 255.0));
    } else {
      ostream_ << BgRgb8(r, g, b);
    }
  }
  return *this;
}

Stream& Stream::SetCursor(size_t row, size_t column) {
  ostream_ << "\e[" << row << ";" << column << "H";
  return *this;
}

Stream& Stream::CursorUp(size_t n) {
  ostream_ << "\e[" << n << "A";
  return *this;
}

Stream& Stream::CursorDown(size_t n) {
  ostream_ << "\e[" << n << "B";
  return *this;
}

Stream& Stream::CursorForward(size_t n) {
  ostream_ << "\e[" << n << "C";
  return *this;
}

Stream& Stream::CursorBackward(size_t n) {
  ostream_ << "\e[" << n << "D";
  return *this;
}

Stream& Stream::SaveCursor() {
  ostream_ << "\e[s";
  return *this;
}

Stream& Stream::RestoreCursor() {
  ostream_ << "\e[u";
  return *this;
}

Stream& Stream::SaveCursorAndAttributes() {
  ostream_ << "\e7";
  return *this;
}

Stream& Stream::RestoreCursorAndAttributes() {
  ostream_ << "\e8";
  return *this;
}

Stream& Stream::ScrollScreen() {
  ostream_ << "\e[r";
  return *this;
}

Stream& Stream::ScrollScreen(size_t start, size_t end) {
  ostream_ << "\e[" << start << ";" << end << "r";
  return *this;
}

Stream& Stream::ScrollDown() {
  ostream_ << "\eD";
  return *this;
}

Stream& Stream::ScrollUp() {
  ostream_ << "\eM";
  return *this;
}

Stream& Stream::SetTab() {
  ostream_ << "\eH";
  return *this;
}

Stream& Stream::ClearTab() {
  ostream_ << "\e[g";
  return *this;
}

Stream& Stream::ClearAllTab() {
  ostream_ << "\e[3g";
  return *this;
}

Stream& Stream::EraseEndOfLine() {
  ostream_ << "\e[K";
  return *this;
}

Stream& Stream::EraseStartOfLine() {
  ostream_ << "\e[1K";
  return *this;
}

Stream& Stream::EraseEntireLine() {
  ostream_ << "\e[2K";
  return *this;
}

Stream& Stream::EraseDown() {
  ostream_ << "\e[J";
  return *this;
}

Stream& Stream::EraseUp() {
  ostream_ << "\e[1J";
  return *this;
}

Stream& Stream::EraseScreen() {
  ostream_ << "\e[2J";
  return *this;
}

}  // namespace console