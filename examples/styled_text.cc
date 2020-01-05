// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>
#include <sstream>

#include "console/stream.h"

int main() {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

#define PrintStyledText(style, text) \
  {                                  \
    console::Stream stream;          \
    stream.style();                  \
    std::cout << text;               \
  }                                  \
  std::cout << " "
  PrintStyledText(Bold, "bold");
  PrintStyledText(Dim, "dim");
  PrintStyledText(Italic, "italic");
  PrintStyledText(Underline, "underline");
  PrintStyledText(Inverse, "inverse");
  PrintStyledText(StrikeThrough, "strikethrough");
  PrintStyledText(Black, "black");
  PrintStyledText(Red, "red");
  PrintStyledText(Green, "green");
  PrintStyledText(Yellow, "yellow");
  PrintStyledText(Blue, "blue");
  PrintStyledText(Magenta, "magenta");
  PrintStyledText(Cyan, "cyan");
  PrintStyledText(White, "white");
  PrintStyledText(LightBlack, "lightblack");
  PrintStyledText(LightRed, "lightred");
  PrintStyledText(LightGreen, "lightgreen");
  PrintStyledText(LightYellow, "lightyellow");
  PrintStyledText(LightBlue, "lightblue");
  PrintStyledText(LightMagenta, "lightmagenta");
  PrintStyledText(LightCyan, "lightcyan");
  PrintStyledText(LightWhite, "lightwhite");
  PrintStyledText(BgBlack, "black");
  PrintStyledText(BgRed, "red");
  PrintStyledText(BgGreen, "green");
  PrintStyledText(BgYellow, "yellow");
  PrintStyledText(BgBlue, "blue");
  PrintStyledText(BgMagenta, "magenta");
  PrintStyledText(BgCyan, "cyan");
  PrintStyledText(BgWhite, "white");
  PrintStyledText(BgLightBlack, "lightblack");
  PrintStyledText(BgLightRed, "lightred");
  PrintStyledText(BgLightGreen, "lightgreen");
  PrintStyledText(BgLightYellow, "lightyellow");
  PrintStyledText(BgLightBlue, "lightblue");
  PrintStyledText(BgLightMagenta, "lightmagenta");
  PrintStyledText(BgLightCyan, "lightcyan");
  PrintStyledText(BgLightWhite, "lightwhite");
  console::Stream stream;
  stream.EraseEndOfLine();
  std::cout << std::endl;
}