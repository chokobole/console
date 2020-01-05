// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_ANIMATION_H_
#define CONSOLE_ANIMATION_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "color/color.h"
#include "console/export.h"

namespace console {

class CONSOLE_EXPORT Animation {
 public:
  typedef std::function<void()> OnAnimationStart;
  typedef std::function<void(size_t)> OnAnimationWillUpdate;
  typedef std::function<void(size_t)> OnAnimationDidUpdate;
  typedef std::function<void()> OnAnimationEnd;

  Animation();
  virtual ~Animation();

  void set_on_animation_start(OnAnimationStart on_animation_start);
  void set_on_animation_will_update(
      OnAnimationWillUpdate on_animation_will_update);
  void set_on_animation_did_update(
      OnAnimationDidUpdate on_animation_did_update);
  void set_on_animation_end(OnAnimationEnd on_animation_end);
  void set_repeat(bool repeat);

  void Update();

 protected:
  friend class AnimationGroup;

  virtual bool ShouldUpdate() = 0;
  virtual void DoUpdate() = 0;

  OnAnimationStart on_animation_start_;
  OnAnimationWillUpdate on_animation_will_update_;
  OnAnimationDidUpdate on_animation_did_update_;
  OnAnimationEnd on_animation_end_;
  size_t current_frame_ = 0;
  bool repeat_ = false;
  bool started_ = false;
  bool ended_ = false;
};

class CONSOLE_EXPORT AnimationGroup : public Animation {
 public:
  AnimationGroup();
  ~AnimationGroup() override;

  void AddAnimation(std::unique_ptr<Animation> animation);

 private:
  bool ShouldUpdate() override;
  void DoUpdate() override;

  std::vector<std::unique_ptr<Animation>> animations_;
};

class CONSOLE_EXPORT TextAnimation : public Animation {
 public:
  TextAnimation();
  ~TextAnimation() override;

  void set_text(const std::string& text);
  void set_text(std::string&& text);

  const std::string& text() const;

 protected:
  std::string text_;
};

class CONSOLE_EXPORT FlowTextAnimation : public TextAnimation {
 public:
  FlowTextAnimation();
  ~FlowTextAnimation() override;

  void set_colors(const std::vector<color::Rgb>& colors);
  void set_colors(std::vector<color::Rgb>&& colors);

  const std::vector<color::Rgb>& colors() const;

 protected:
  bool ShouldUpdate() override;
  void DoUpdate() override;

  std::vector<color::Rgb> colors_;
};

class CONSOLE_EXPORT NeonTextAnimation : public TextAnimation {
 public:
  NeonTextAnimation();
  ~NeonTextAnimation() override;

  void set_colors(const std::vector<color::Rgb>& colors);
  void set_colors(std::vector<color::Rgb>&& colors);

  const std::vector<color::Rgb>& colors() const;

 protected:
  bool ShouldUpdate() override;
  void DoUpdate() override;

  std::vector<color::Rgb> colors_;
};

class CONSOLE_EXPORT KaraokeTextAnimation : public TextAnimation {
 public:
  KaraokeTextAnimation();
  ~KaraokeTextAnimation() override;

  void set_color(color::Rgb color);

  color::Rgb color() const;

 protected:
  bool ShouldUpdate() override;
  void DoUpdate() override;

  color::Rgb color_;
};

class CONSOLE_EXPORT RadarTextAnimation : public TextAnimation {
 public:
  RadarTextAnimation();
  ~RadarTextAnimation() override;

  void set_colors(const std::vector<color::Rgb>& colors);
  void set_colors(std::vector<color::Rgb>&& colors);

  const std::vector<color::Rgb>& colors() const;

 protected:
  bool ShouldUpdate() override;
  void DoUpdate() override;

  std::vector<color::Rgb> colors_;
};

}  // namespace console

#endif  // CONSOLE_ANIMATION_H_