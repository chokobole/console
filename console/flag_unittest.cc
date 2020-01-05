// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/flag.h"

#include "gtest/gtest.h"

namespace console {

TEST(FlagTest, NameRule) {
  bool value;
  BoolFlag flag(&value);
  flag.set_short_name("-a");
  EXPECT_EQ(flag.short_name(), "-a");
  flag.set_short_name("--a");
  EXPECT_EQ(flag.short_name(), "-a");
  flag.set_short_name("a");
  EXPECT_EQ(flag.short_name(), "-a");

  flag.set_long_name("--a");
  EXPECT_EQ(flag.long_name(), "--a");
  flag.set_long_name("-a");
  EXPECT_EQ(flag.long_name(), "--a");
  flag.set_long_name("a");
  EXPECT_EQ(flag.long_name(), "--a");

  flag.set_name("a");
  EXPECT_EQ(flag.name(), "a");
  flag.set_name("-a");
  EXPECT_EQ(flag.name(), "a");
  flag.set_name("--a");
  EXPECT_EQ(flag.name(), "a");
}

TEST(FlagTest, ParseValue) {
  bool bool_value = false;
  std::string reason;
  BoolFlag bool_flag(&bool_value);
  bool_flag.set_name("bool_flag");
  EXPECT_TRUE(bool_flag.ParseValue("", &reason));
  EXPECT_EQ(reason, "");
  EXPECT_TRUE(bool_value);

  reason.clear();
  int16_t int16_value;
  Int16Flag int16_flag(&int16_value);
  bool_flag.set_name("int16_flag");
  EXPECT_TRUE(int16_flag.ParseValue("123", &reason));
  EXPECT_EQ(reason, "");
  EXPECT_EQ(int16_value, 123);
  EXPECT_FALSE(int16_flag.ParseValue("a", &reason));
  EXPECT_EQ(reason, "failed to convert int (\"a\")");
  EXPECT_FALSE(int16_flag.ParseValue("40000", &reason));
  EXPECT_EQ(reason, "40000 is outside of its range");
  EXPECT_EQ(int16_value, 123);

  reason.clear();
  std::string string_value;
  StringFlag string_flag(&string_value);
  string_flag.set_name("string_flag");
  EXPECT_TRUE(string_flag.ParseValue("abc", &reason));
  EXPECT_EQ(reason, "");
  EXPECT_EQ(string_value, "abc");
  EXPECT_FALSE(string_flag.ParseValue("", &reason));
  EXPECT_EQ(reason, "input is empty");
  EXPECT_EQ(string_value, "abc");
}

TEST(FlagParserTest, Validate) {
  {
    FlagParser parser;
    uint16_t value;
    parser.AddFlag<Uint16Flag>(&value);
    EXPECT_FALSE(parser.Validate());
    EXPECT_EQ(parser.error_message(), "Flag should be positional or optional.");
  }
  {
    FlagParser parser;
    uint16_t value;
    parser.AddFlag<Uint16Flag>(&value).set_name("value").set_short_name("-v");
    EXPECT_FALSE(parser.Validate());
    EXPECT_EQ(parser.error_message(),
              "\"value\" is positional and optional, please choose either one "
              "of them.");
  }
  {
    FlagParser parser;
    parser.AddSubParser().set_short_name("-a");
    EXPECT_FALSE(parser.Validate());
    EXPECT_EQ(parser.error_message(), "Subparser \"-a\" should be positional.");
  }
  {
    FlagParser parser;
    uint16_t value;
    parser.AddFlag<Uint16Flag>(&value).set_short_name("-v");
    parser.AddFlag<Uint16Flag>(&value).set_name("value");
    EXPECT_FALSE(parser.Validate());
    EXPECT_EQ(parser.error_message(),
              "\"value\" should be before any optional arguments.");
  }
  {
    FlagParser parser;
    bool value;
    parser.AddFlag<BoolFlag>(&value).set_name("value");
    EXPECT_FALSE(parser.Validate());
    EXPECT_EQ(parser.error_message(),
              "\"value\" can't parse a value, how about considering using "
              "set_short_name() or set_long_name()?");
  }
  {
    FlagParser parser;
    uint16_t value;
    parser.AddSubParser().set_name("a");
    parser.AddFlag<Uint16Flag>(&value).set_name("value");
    EXPECT_FALSE(parser.Validate());
    EXPECT_EQ(parser.error_message(),
              "\"value\" can't be positional if the parser has "
              "subparser, how about considering using "
              "set_short_name() or set_long_name()?");
  }
}

TEST(FlagParserTest, UndefinedArgument) {
  FlagParser parser;
  uint16_t value;
  parser.AddFlag<Uint16Flag>(&value).set_long_name("--value");
  {
    const char* argv[] = {"program", "--v", "16"};
    EXPECT_FALSE(parser.Parse(3, const_cast<char**>(argv)));
    EXPECT_EQ(parser.error_message(), "met unknown argument: \"--v\".");
  }
  {
    const char* argv[] = {"program", "--val", "16"};
    EXPECT_FALSE(parser.Parse(3, const_cast<char**>(argv)));
    EXPECT_EQ(parser.error_message(),
              "met unknown argument: \"--val\", maybe you mean \"--value\"?");
  }
}

TEST(FlagParserTest, DefaultValue) {
  FlagParser parser;
  uint16_t value;
  parser.AddFlag<Uint16Flag>(&value, static_cast<uint16_t>(12))
      .set_name("flag");
  {
    const char* argv[] = {"program"};
    EXPECT_TRUE(parser.Parse(1, const_cast<char**>(argv)));
    EXPECT_EQ(value, 12);
  }
}

TEST(FlagParserTest, PositionalArgumtens) {
  FlagParser parser;
  uint16_t value;
  uint16_t value2;
  parser.AddFlag<Uint16Flag>(&value).set_name("flag");
  parser.AddFlag<Uint16Flag>(&value2).set_name("flag2");
  {
    const char* argv[] = {"program", "12"};
    EXPECT_FALSE(parser.Parse(2, const_cast<char**>(argv)));
    EXPECT_EQ(parser.error_message(), "\"flag2\" is positional, but not set.");
  }
  {
    const char* argv[] = {"program", "12", "34"};
    EXPECT_TRUE(parser.Parse(3, const_cast<char**>(argv)));
  }
  EXPECT_EQ(value, 12);
  EXPECT_EQ(value2, 34);
}

TEST(FlagParserTest, RequiredOptionalArguments) {
  FlagParser parser;
  uint16_t value;
  uint16_t value2;
  parser.AddFlag<Uint16Flag>(&value).set_short_name("-a");
  parser.AddFlag<Uint16Flag>(&value2).set_short_name("-b").set_required();
  {
    const char* argv[] = {"program", "-a", "12"};
    EXPECT_FALSE(parser.Parse(3, const_cast<char**>(argv)));
    EXPECT_EQ(parser.error_message(), "\"-b\" is required, but not set.");
  }
  {
    const char* argv[] = {"program", "-b", "34"};
    EXPECT_TRUE(parser.Parse(3, const_cast<char**>(argv)));
  }
  EXPECT_EQ(value2, 34);
  {
    const char* argv[] = {"program", "-a", "56", "-b", "78"};
    EXPECT_TRUE(parser.Parse(5, const_cast<char**>(argv)));
  }
  EXPECT_EQ(value, 56);
  EXPECT_EQ(value2, 78);
}

TEST(FlagParserTest, VectorFlag) {
  FlagParser parser;
  std::vector<int> numbers;
  parser.AddFlag<Flag<std::vector<int>>>(&numbers).set_short_name("-a");
  {
    const char* argv[] = {"program", "-a", "1", "-a", "2", "-a", "3"};
    EXPECT_TRUE(parser.Parse(7, const_cast<char**>(argv)));
    EXPECT_EQ(numbers.size(), 3);
    for (size_t i = 0; i < numbers.size(); ++i) {
      EXPECT_EQ(numbers[i], (i + 1));
    }
  }
}

TEST(FlagParserTest, CustomParseValueCallback) {
  FlagParser parser;
  std::string value;
  parser
      .AddFlag<StringFlag>(
          [&value](absl::string_view arg, std::string* reason) {
            if (arg == "cat" || arg == "dog") {
              value = std::string(arg);
              return true;
            }
            *reason = absl::StrFormat("%s is not either cat or dog", arg);
            return false;
          })
      .set_short_name("-a");
  {
    const char* argv[] = {"program", "-a", "pig"};
    EXPECT_FALSE(parser.Parse(3, const_cast<char**>(argv)));
    EXPECT_EQ(parser.error_message(),
              "-a is failed to parse: (reason: pig is not either cat or dog).");
  }
  {
    const char* argv[] = {"program", "-a", "cat"};
    EXPECT_TRUE(parser.Parse(3, const_cast<char**>(argv)));
    EXPECT_EQ(value, "cat");
  }
}

}  // namespace console