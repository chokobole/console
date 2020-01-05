// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "console/animation.h"
#include "console/stream.h"

class ProgressAnimation : public console::Animation {
 public:
  explicit ProgressAnimation(size_t total_frames)
      : total_frames_(total_frames) {}

 private:
  bool ShouldUpdate() override { return current_frame_ < total_frames_; }

  void DoUpdate() override {
    int did = static_cast<double>(current_frame_ + 1) / total_frames_ * 50;
    for (int i = 0; i < did; ++i) {
      std::cout << "=";
    }
    std::cout << ">";
    for (int i = 0; i < 50 - did; ++i) {
      std::cout << " ";
    }
    std::cout << "[" << (current_frame_ + 1) << " / " << total_frames_ << "]\n";
  }

  size_t total_frames_;
};

int main() {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  bool updating = true;
  ProgressAnimation animation(100);
  animation.set_on_animation_end([&updating] { updating = false; });
  animation.set_on_animation_will_update([](size_t framenum) {
    if (framenum == 0) return;
    console::Stream stream;
    stream.CursorUp(1);
    stream.EraseEndOfLine();
  });

  do {
    animation.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  } while (updating);

  return 0;
}