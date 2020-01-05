// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "color/colormap.h"
#include "color/named_color.h"
#include "console/animation.h"
#include "console/stream.h"

int main() {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  console::Stream stream;
  color::Colormap colormap;
  std::vector<color::Rgb> rainbow_colors, grayscale_colors;
  colormap.Rainbow(30, &rainbow_colors);
  colormap.Greys(5, &grayscale_colors);

  const char* text =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit\n";

  std::unique_ptr<console::FlowTextAnimation> flow_animation(
      new console::FlowTextAnimation());
  flow_animation->set_repeat(true);
  flow_animation->set_colors(rainbow_colors);
  flow_animation->set_text(text);
  flow_animation->set_on_animation_will_update(
      [](size_t) { std::cout << "flow animation: "; });
  std::unique_ptr<console::NeonTextAnimation> neon_animation(
      new console::NeonTextAnimation());
  neon_animation->set_repeat(true);
  neon_animation->set_colors({color::kPurple, color::kGray});
  neon_animation->set_text(text);
  neon_animation->set_on_animation_will_update(
      [](size_t) { std::cout << "neon animation: "; });
  std::unique_ptr<console::KaraokeTextAnimation> karaoke_animation(
      new console::KaraokeTextAnimation());
  karaoke_animation->set_repeat(true);
  karaoke_animation->set_color(color::kOrange);
  karaoke_animation->set_text(text);
  karaoke_animation->set_on_animation_will_update(
      [](size_t) { std::cout << "karaoke animation: "; });
  std::unique_ptr<console::RadarTextAnimation> radar_animation(
      new console::RadarTextAnimation());
  radar_animation->set_repeat(true);
  radar_animation->set_colors(grayscale_colors);
  radar_animation->set_text(text);
  radar_animation->set_on_animation_will_update(
      [](size_t) { std::cout << "radar animation: "; });
  console::AnimationGroup group;
  group.AddAnimation(std::move(flow_animation));
  group.AddAnimation(std::move(neon_animation));
  group.AddAnimation(std::move(karaoke_animation));
  group.AddAnimation(std::move(radar_animation));
  group.set_on_animation_will_update([](size_t framenum) {
    if (framenum == 0) return;
    console::Stream stream;
    stream.CursorUp(4);
    stream.EraseEndOfLine();
  });

  while (true) {
    group.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}