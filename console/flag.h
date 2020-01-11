// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_FLAG_H_
#define CONSOLE_FLAG_H_

#include <stddef.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>
#include <vector>

#include "base/strings/string_util.h"
#include "console/export.h"
#include "console/flag_forward.h"
#include "console/flag_value_traits.h"

namespace console {

class FlagParser;

// FlagBase must have |short_name_|, |long_name_| or |name_|.
// All the names should contain alphabet or digit.
// But |short_name_| has a prefix "-" and |long_name_| has a prefix "--".
// |long_name_| and |short_name_| can be set together, but |name_| shouldn't.
class CONSOLE_EXPORT FlagBase {
 public:
  FlagBase();
  FlagBase(const FlagBase& other) = delete;
  FlagBase& operator=(const FlagBase& other) = delete;
  virtual ~FlagBase();

  FlagBase& set_short_name(absl::string_view short_name);
  FlagBase& set_long_name(absl::string_view long_name);
  FlagBase& set_name(absl::string_view name);
  FlagBase& set_help(absl::string_view help);
  FlagBase& set_required();
  FlagBase& set_sequential();

  const std::string& short_name() const;
  const std::string& long_name() const;
  const std::string& name() const;
  const std::string& help() const;

  // Returns true if the flag was marked with required.
  bool is_required() const;
  // Returns true if the flag was marked with sequential.
  bool is_sequential() const;
  // Returns true |name_| was set.
  bool is_positional() const;
  // Returns true |short_name_| or |long_name_| was set.
  bool is_optional() const;
  // Returns true if a value was set.
  bool is_set() const;

 protected:
  friend class FlagParser;
  friend class Autocompletion;

  // Returns |name_| if it is positional.
  // Otherwise, it returns |long_name_| if it is not empty.
  // Returns |short_name_| if |long_name_| is empty.
  const std::string& display_name() const;
  std::string display_usage() const;
  std::string display_help(int help_start = 0) const;

  bool ConsumeNamePrefix(FlagParser& parser, absl::string_view* arg) const;

  virtual bool IsSubParser() const;
  // Returns true if underlying type of Flag<T>, in other words, T is bool.
  virtual bool NeedsValue() const = 0;
  virtual bool ParseValue(absl::string_view arg, std::string* reason) = 0;

  std::string short_name_;
  std::string long_name_;
  std::string name_;
  std::string help_;
  bool is_required_ = false;
  bool is_sequential_ = false;
  bool is_set_ = false;
};

template <typename T>
class Flag : public FlagBase {
 public:
  typedef T value_type;
  typedef std::function<bool(absl::string_view, std::string*)>
      ParseValueCallback;

  explicit Flag(T* value) : value_(value) {}
  Flag(T* value, const T& default_value) : value_(value) {
    set_value(default_value);
  }
  Flag(ParseValueCallback parse_value_callback)
      : parse_value_callback_(parse_value_callback) {}
  Flag(const Flag& other) = delete;
  Flag& operator=(const Flag& other) = delete;

  void set_value(const T& value) {
    is_set_ = true;
    *value_ = value;
  }

  const T* value() const { return value_; }

  bool ParseValue(absl::string_view arg, std::string* reason) override;

 private:
  bool NeedsValue() const override { return !std::is_same<T, bool>::value; }

  T* value_ = nullptr;
  ParseValueCallback parse_value_callback_;
};

template <typename T>
bool Flag<T>::ParseValue(absl::string_view arg, std::string* reason) {
  if (parse_value_callback_) {
    bool ret = parse_value_callback_(arg, reason);
    if (ret) {
      is_set_ = true;
    }
    return ret;
  }

  if (FlagValueTraits<T>::ParseValue(arg, value_, reason)) {
    is_set_ = true;
    return true;
  }
  return false;
}

class SubParser;

class CONSOLE_EXPORT FlagParser {
 public:
  FlagParser();
  virtual ~FlagParser();

  // Sets program name to display.
  // For example, when there are --foo, --bar flags and type --help,
  // It shows |program_name| [--foo] [--bar]
  void set_program_name(absl::string_view program_name);
  const std::string& program_name() const;
  const std::string& error_message();

  template <typename T, typename value_type = typename Flag<T>::value_type>
  FlagBase& AddFlag(value_type* value) {
    std::unique_ptr<FlagBase> flag(new T(value));
    flags_.push_back(std::move(flag));
    return *flags_.back().get();
  }

  template <typename T, typename value_type = typename Flag<T>::value_type>
  FlagBase& AddFlag(value_type* value, const value_type& default_value) {
    std::unique_ptr<FlagBase> flag(new T(value, default_value));
    flags_.push_back(std::move(flag));
    return *flags_.back().get();
  }

  template <typename T,
            typename ParseValueCallback = typename Flag<T>::ParseValueCallback>
  FlagBase& AddFlag(ParseValueCallback parse_value_callback) {
    std::unique_ptr<FlagBase> flag(new T(parse_value_callback));
    flags_.push_back(std::move(flag));
    return *flags_.back().get();
  }

  FlagBase& AddFlag(std::unique_ptr<FlagBase> flag_base);

  SubParser& AddSubParser();

  bool Validate();

  bool Parse(int argc, char** argv, int from = 1);

  // It marks virtual so that users can make custom help messages.
  virtual std::string help_message();

 protected:
  friend class Autocompletion;

  absl::string_view current();
  bool ConsumeEqualOrProceed(absl::string_view* arg);
  void Proceed();

  // Internally it measures Levenshtein distance among arguments.
  bool FindTheMostSimilarFlag(absl::string_view input,
                              absl::string_view* output);

  std::string program_name_;
  int argc_;
  int current_idx_;
  char** argv_;
  std::string error_message_;
  std::vector<std::unique_ptr<FlagBase>> flags_;
};

class CONSOLE_EXPORT SubParser : public FlagBase, public FlagParser {
 public:
  SubParser();
  ~SubParser();
  SubParser(const FlagBase& other) = delete;
  SubParser& operator=(const FlagBase& other) = delete;

  // FlagBase methods
  bool IsSubParser() const override;
  bool NeedsValue() const override;
  bool ParseValue(absl::string_view arg, std::string* reason) override;
};

}  // namespace console

#endif  // CONSOLE_FLAG_H_
