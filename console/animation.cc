// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/animation.h"

#include <algorithm>

#include "console/stream.h"

namespace console {

Animation::Animation() = default;

Animation::~Animation() = default;

void Animation::set_on_animation_start(OnAnimationStart on_animation_start) {
  on_animation_start_ = on_animation_start;
}

void Animation::set_on_animation_will_update(
    OnAnimationWillUpdate on_animation_will_update) {
  on_animation_will_update_ = on_animation_will_update;
}

void Animation::set_on_animation_did_update(
    OnAnimationDidUpdate on_animation_did_update) {
  on_animation_did_update_ = on_animation_did_update;
}

void Animation::set_on_animation_end(OnAnimationEnd on_animation_end) {
  on_animation_end_ = on_animation_end;
}

void Animation::set_repeat(bool repeat) { repeat_ = repeat; }

void Animation::Update() {
  if (ended_) return;
  if (!ShouldUpdate()) return;

  if (!started_) {
    started_ = true;
    if (on_animation_start_) on_animation_start_();
  }

  if (on_animation_will_update_) {
    on_animation_will_update_(current_frame_);
  }

  DoUpdate();

  if (on_animation_did_update_) {
    on_animation_did_update_(current_frame_);
  }

  current_frame_++;

  if (ended_) {
    if (on_animation_end_) on_animation_end_();
  }
}

AnimationGroup::AnimationGroup() = default;

AnimationGroup::~AnimationGroup() = default;

void AnimationGroup::AddAnimation(std::unique_ptr<Animation> animation) {
  animations_.push_back(std::move(animation));
}

bool AnimationGroup::ShouldUpdate() { return true; }

void AnimationGroup::DoUpdate() {
  bool ended = false;

  for (auto& animation : animations_) {
    animation->Update();
    ended &= animation->ended_;
  }

  ended_ = ended;
}

TextAnimation::TextAnimation() = default;

TextAnimation::~TextAnimation() = default;

void TextAnimation::set_text(const std::string& text) { text_ = text; }

void TextAnimation::set_text(std::string&& text) { text_ = std::move(text); }

const std::string& TextAnimation::text() const { return text_; }

FlowTextAnimation::FlowTextAnimation() = default;

FlowTextAnimation::~FlowTextAnimation() = default;

void FlowTextAnimation::set_colors(const std::vector<color::Rgb>& colors) {
  colors_ = colors;
}

void FlowTextAnimation::set_colors(std::vector<color::Rgb>&& colors) {
  colors_ = std::move(colors);
}

const std::vector<color::Rgb>& FlowTextAnimation::colors() const {
  return colors_;
}

bool FlowTextAnimation::ShouldUpdate() {
  if (colors_.size() == 0) return false;
  if (text_.length() == 0) return false;
  return true;
}

void FlowTextAnimation::DoUpdate() {
  console::Stream stream;
  size_t c = current_frame_ % colors_.size();
  for (size_t i = 0; i < text_.length(); ++i) {
    stream.Rgb(colors_[(c + i) % colors_.size()]);
    std::cout << text_[i];
  }

  if (!repeat_) {
    if (current_frame_ == text_.length() - 1) {
      ended_ = true;
    }
  }
}

NeonTextAnimation::NeonTextAnimation() = default;

NeonTextAnimation::~NeonTextAnimation() = default;

void NeonTextAnimation::set_colors(const std::vector<color::Rgb>& colors) {
  colors_ = colors;
}

void NeonTextAnimation::set_colors(std::vector<color::Rgb>&& colors) {
  colors_ = std::move(colors);
}

const std::vector<color::Rgb>& NeonTextAnimation::colors() const {
  return colors_;
}

bool NeonTextAnimation::ShouldUpdate() {
  if (colors_.size() == 0) return false;
  if (text_.length() == 0) return false;
  return true;
}

void NeonTextAnimation::DoUpdate() {
  console::Stream stream;
  stream.Rgb(colors_[current_frame_ % colors_.size()]);
  std::cout << text_;

  if (!repeat_) {
    if (current_frame_ == colors_.size() - 1) {
      ended_ = true;
    }
  }
}

KaraokeTextAnimation::KaraokeTextAnimation() = default;

KaraokeTextAnimation::~KaraokeTextAnimation() = default;

void KaraokeTextAnimation::set_color(color::Rgb color) { color_ = color; }

color::Rgb KaraokeTextAnimation::color() const { return color_; }

bool KaraokeTextAnimation::ShouldUpdate() {
  if (text_.length() == 0) return false;
  return true;
}

void KaraokeTextAnimation::DoUpdate() {
  size_t i = 0;
  {
    console::Stream stream;
    stream.Rgb(color_);
    for (; i < current_frame_ % text_.length(); ++i) {
      std::cout << text_[i];
    }
  }
  for (; i < text_.length(); ++i) {
    std::cout << text_[i];
  }

  if (!repeat_) {
    if (current_frame_ == text_.length() - 1) {
      ended_ = true;
    }
  }
}

RadarTextAnimation::RadarTextAnimation() = default;

RadarTextAnimation::~RadarTextAnimation() = default;

void RadarTextAnimation::set_colors(const std::vector<color::Rgb>& colors) {
  colors_ = colors;
}

void RadarTextAnimation::set_colors(std::vector<color::Rgb>&& colors) {
  colors_ = std::move(colors);
}

const std::vector<color::Rgb>& RadarTextAnimation::colors() const {
  return colors_;
}

bool RadarTextAnimation::ShouldUpdate() {
  if (colors_.size() == 0) return false;
  if (text_.length() == 0) return false;
  return true;
}

void RadarTextAnimation::DoUpdate() {
  console::Stream stream;
  stream.Conceal();
  size_t i = 0;
  size_t offset = current_frame_ % text_.length();
  for (; i < offset; ++i) {
    std::cout << text_[i];
  }
  stream.ConcealOff();
  for (; i < text_.length() && i - offset < colors_.size(); ++i) {
    stream.Rgb(colors_[i - offset]);
    std::cout << text_[i];
  }
  stream.ColorOff();
  stream.Conceal();
  for (; i < text_.length(); ++i) {
    std::cout << text_[i];
  }

  if (!repeat_) {
    if (current_frame_ == text_.length() - 1) {
      ended_ = true;
    }
  }
}

}  // namespace console