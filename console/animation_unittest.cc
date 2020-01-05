// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/animation.h"

#include "color/named_color.h"
#include "gtest/gtest.h"

namespace console {

namespace {

class TestAnimation : public TextAnimation {
 public:
  typedef std::function<bool()> ShouldUpdateCallback;
  typedef std::function<void()> DoUpdateCallback;

  void set_should_update_callback(ShouldUpdateCallback should_update_callback) {
    should_update_callback_ = should_update_callback;
  }

  void set_do_update_callback(DoUpdateCallback do_update_callback) {
    do_update_callback_ = do_update_callback;
  }

  bool ShouldUpdate() override {
    if (!should_update_callback_) return true;
    return should_update_callback_();
  }

  void DoUpdate() override {
    if (do_update_callback_) do_update_callback_();
  }

  void set_end() { ended_ = true; }

 private:
  ShouldUpdateCallback should_update_callback_;
  DoUpdateCallback do_update_callback_;
};

}  // namespace

#define SETUP_CALLBACKS(animation)                                    \
  bool started = false;                                               \
  bool ended = false;                                                 \
  size_t will_update = 0;                                             \
  size_t did_update = 0;                                              \
  animation.set_on_animation_start([&started]() { started = true; }); \
  animation.set_on_animation_end([&ended]() { ended = true; });       \
  animation.set_on_animation_will_update(                             \
      [&will_update](size_t) { will_update++; });                     \
  animation.set_on_animation_did_update([&did_update](size_t) { did_update++; })

TEST(AnimationTest, Callback) {
  TestAnimation animation;
  SETUP_CALLBACKS(animation);

  size_t count = 0;
  animation.set_do_update_callback([&count, &animation]() {
    count++;
    if (count == 5) {
      animation.set_end();
    }
  });

  animation.Update();
  EXPECT_TRUE(started);
  EXPECT_FALSE(ended);
  EXPECT_EQ(will_update, 1);
  EXPECT_EQ(did_update, 1);
  for (size_t i = 0; i < 4; ++i) {
    animation.Update();
    EXPECT_EQ(will_update, 2 + i);
    EXPECT_EQ(did_update, 2 + i);
  }
  EXPECT_TRUE(ended);
}

TEST(FlowTextAnimationTest, Callback) {
  {
    FlowTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    FlowTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    FlowTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});
    for (size_t i = 0; i < animation.text().length(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      if (i == animation.text().length() - 1) {
        EXPECT_TRUE(ended);
      } else {
        EXPECT_FALSE(ended);
      }
    }
  }
  {
    FlowTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});
    animation.set_repeat(true);
    for (size_t i = 0; i < animation.text().length(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      EXPECT_FALSE(ended);
    }
  }
}

TEST(NeonTextAnimationTest, Callback) {
  {
    NeonTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    NeonTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    NeonTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});
    for (size_t i = 0; i < animation.colors().size(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      if (i == animation.colors().size() - 1) {
        EXPECT_TRUE(ended);
      } else {
        EXPECT_FALSE(ended);
      }
    }
  }
  {
    NeonTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});
    animation.set_repeat(true);
    for (size_t i = 0; i < animation.colors().size(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      EXPECT_FALSE(ended);
    }
  }
}

TEST(KaraokeTextAnimationTest, Callback) {
  {
    KaraokeTextAnimation animation;
    SETUP_CALLBACKS(animation);

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    KaraokeTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_color(color::kBlack);
    for (size_t i = 0; i < animation.text().length(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      if (i == animation.text().length() - 1) {
        EXPECT_TRUE(ended);
      } else {
        EXPECT_FALSE(ended);
      }
    }
  }
  {
    KaraokeTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_color(color::kBlack);
    animation.set_repeat(true);
    for (size_t i = 0; i < animation.text().length(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      EXPECT_FALSE(ended);
    }
  }
}

TEST(RadarTextAnimationTest, Callback) {
  {
    RadarTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    RadarTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});

    animation.Update();
    EXPECT_FALSE(started);
    EXPECT_FALSE(ended);
    EXPECT_EQ(will_update, 0);
    EXPECT_EQ(did_update, 0);
  }
  {
    RadarTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});
    for (size_t i = 0; i < animation.text().length(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      if (i == animation.text().length() - 1) {
        EXPECT_TRUE(ended);
      } else {
        EXPECT_FALSE(ended);
      }
    }
  }
  {
    RadarTextAnimation animation;
    SETUP_CALLBACKS(animation);
    animation.set_text("Hello World\n");
    animation.set_colors({color::kBlack, color::kGray, color::kWhite});
    animation.set_repeat(true);
    for (size_t i = 0; i < animation.text().length(); ++i) {
      animation.Update();
      EXPECT_TRUE(started);
      EXPECT_FALSE(ended);
    }
  }
}

}  // namespace console