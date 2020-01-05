// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_STREAM_H_
#define CONSOLE_STREAM_H_

#include <stdint.h>

#include <iostream>

#include "color/color.h"
#include "console/console.h"
#include "console/export.h"
#include "console/sgr_parameters.h"

namespace console {

class CONSOLE_EXPORT Stream {
 public:
  explicit Stream(std::ostream& ostream_ = std::cout);
  ~Stream();

#define SGR_PARAMETERS_LIST(name, code) Stream& name();
#include "console/sgr_parameters_list.h"
#undef SGR_PARAMETERS_LIST

  Stream& Rgb(color::Rgb rgb);
  Stream& Rgb(uint8_t r, uint8_t g, uint8_t b);
  Stream& BgRgb(color::Rgb rgb);
  Stream& BgRgb(uint8_t r, uint8_t g, uint8_t b);

  // Cursor Conrol
  // Sets the cursor position where subsequent text will begin. If no row/column
  // parameters are provided, the cursor will move to the home
  // position, at the upper left of the screen.
  Stream& SetCursor(size_t row = 0, size_t column = 0);
  // Moves the cursor up by |n| rows.
  Stream& CursorUp(size_t n = 1);
  // Moves the cursor down by |n| rows.
  Stream& CursorDown(size_t n = 1);
  // Moves the cursor forward by |n| columns.
  Stream& CursorForward(size_t n = 1);
  // Moves the cursor backward by |n| columns.
  Stream& CursorBackward(size_t n = 1);
  // Saves current cursor position.
  Stream& SaveCursor();
  // Restores cursor position after the save point.
  Stream& RestoreCursor();
  // Save current cursor position and attributes.
  Stream& SaveCursorAndAttributes();
  // Restores cursor position after the save point and attributes.
  Stream& RestoreCursorAndAttributes();

  // Scrolling
  // Enable scrolling for entire display.
  Stream& ScrollScreen();
  // Enable scrolling from row |start| to row |end|.
  Stream& ScrollScreen(size_t start, size_t end);
  // Scroll display down one line.
  Stream& ScrollDown();
  // Scroll display up one line.
  Stream& ScrollUp();

  // Tab Control
  // Sets a tab at the current position.
  Stream& SetTab();
  // Clears tab at the current position.
  Stream& ClearTab();
  // Clears all tabs.
  Stream& ClearAllTab();

  // Erasing Text
  // Erases from the current cursor position to the end of the current line.
  Stream& EraseEndOfLine();
  // Erases from the current cursor position to the start of the current line.
  Stream& EraseStartOfLine();
  // Erases the entire currnet line.
  Stream& EraseEntireLine();
  // Erases the screen from the current line down to the bottom of the screen.
  Stream& EraseDown();
  // Erases the screen from the current line up to the top of the screen.
  Stream& EraseUp();
  // Erases the screen with the background colour and moves the cursor to home.
  Stream& EraseScreen();

 private:
  std::ostream& ostream_;
  Console::Info console_info_;
};

}  // namespace console

#endif  // CONSOLE_STREAM_H_