// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "console/flag.h"

#include <algorithm>

#include "absl/strings/ascii.h"
#include "absl/strings/substitute.h"
#include "base/strings/string_util.h"

namespace console {

namespace {

constexpr const int kDefaultLineWidth = 50;
constexpr const int kDefaultHelpStart = 20;

bool ContainsOnlyAsciiAlphaOrDigitOrUndderscore(absl::string_view text) {
  const char* p = text.data();
  const char* limit = p + text.size();
  while (p < limit) {
    const char c = *p;
    if (!(absl::ascii_isalpha(c) || absl::ascii_isdigit(c) || c == '_'))
      return false;
    p++;
  }

  return true;
}

size_t AppendAndDecreaseLength(std::ostream& ss, absl::string_view text) {
  ss << text;
  return text.length();
}

std::string AlignAtIndexAndAppend(std::stringstream& ss, absl::string_view text,
                                  int index, int index2) {
  int final_index = index;
  if (final_index <= 0) {
    ss << std::endl;
    final_index = index2;
  }
  ss << std::string(final_index, ' ') << text;
  return ss.str();
}

struct Costs {
  Costs(size_t size) : size(size) {
    if (size > 256) {
      costs = new size_t[size];
    } else {
      costs = costs_on_stack;
    }
    Init();
  }

  void Init() {
    for (size_t i = 0; i < size; ++i) {
      costs[i] = i;
    }
  }

  ~Costs() {
    if (size > 256) {
      delete[] costs;
    }
  }

  size_t& operator[](size_t i) { return costs[i]; }

  size_t size;
  size_t* costs = nullptr;
  size_t costs_on_stack[256];
};

// This was taken and modified from
// https://rosettacode.org/wiki/Levenshtein_distance#C.2B.2B
size_t GetLevenshteinDistance(absl::string_view s1, absl::string_view s2) {
  size_t m = s1.size();
  size_t n = s2.size();

  if (m == 0) return n;
  if (n == 0) return m;

  Costs costs(n + 1);

  size_t i = 0;
  for (; i < m; ++i) {
    costs[0] = i + 1;
    size_t corner = i;
    size_t j = 0;
    char c = s1[i];
    for (; j < n; ++j) {
      size_t upper = costs[j + 1];
      if (c == s2[j]) {
        costs[j + 1] = corner;
      } else {
        size_t t = upper < corner ? upper : corner;
        costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
      }

      corner = upper;
    }
  }

  return costs[n];
}

}  // namespace

FlagBase::FlagBase() = default;

FlagBase::~FlagBase() = default;

FlagBase& FlagBase::set_short_name(absl::string_view short_name) {
  absl::string_view text = short_name;
  if (!base::ConsumePrefix(&text, "-")) return *this;
  if (!ContainsOnlyAsciiAlphaOrDigitOrUndderscore(text)) return *this;

  short_name_ = std::string(short_name);
  return *this;
}

FlagBase& FlagBase::set_long_name(absl::string_view long_name) {
  absl::string_view text = long_name;
  if (!base::ConsumePrefix(&text, "--")) return *this;
  if (!ContainsOnlyAsciiAlphaOrDigitOrUndderscore(text)) return *this;

  long_name_ = std::string(long_name);
  return *this;
}

FlagBase& FlagBase::set_name(absl::string_view name) {
  absl::string_view text = name;
  if (!ContainsOnlyAsciiAlphaOrDigitOrUndderscore(text)) return *this;

  name_ = std::string(name);
  return *this;
}

FlagBase& FlagBase::set_help(absl::string_view help) {
  help_ = std::string(help);
  return *this;
}

FlagBase& FlagBase::set_required() {
  is_required_ = true;
  return *this;
}

FlagBase& FlagBase::set_sequential() {
  is_sequential_ = true;
  return *this;
}

const std::string& FlagBase::short_name() const { return short_name_; }

const std::string& FlagBase::long_name() const { return long_name_; }

const std::string& FlagBase::name() const { return name_; }

const std::string& FlagBase::help() const { return help_; }

bool FlagBase::is_required() const { return is_required_; }

bool FlagBase::is_sequential() const { return is_sequential_; }

bool FlagBase::is_positional() const { return !name_.empty(); }

bool FlagBase::is_optional() const {
  return !short_name_.empty() || !long_name_.empty();
}

bool FlagBase::is_set() const { return is_set_; }

const std::string& FlagBase::display_name() const {
  if (!name_.empty()) return name_;
  if (!long_name_.empty()) return long_name_;
  return short_name_;
}

std::string FlagBase::display_help(int help_start) const {
  int remain_len = help_start;
  std::stringstream ss;
  if (is_positional()) {
    remain_len -= AppendAndDecreaseLength(ss, name_);
  } else {
    if (!short_name_.empty()) {
      remain_len -= AppendAndDecreaseLength(ss, short_name_);
    }

    if (!long_name_.empty()) {
      if (!short_name_.empty()) {
        remain_len -= AppendAndDecreaseLength(ss, ", ");
      }
      remain_len -= AppendAndDecreaseLength(ss, long_name_);
    }
  }

  return AlignAtIndexAndAppend(ss, help_, remain_len, help_start);
}

std::string FlagBase::display_usage() const {
  if (is_positional()) {
    return name_;
  } else {
    std::stringstream ss;
    ss << "[";
    if (!short_name_.empty()) {
      ss << short_name_;
    } else {
      ss << long_name_;
    }
    ss << "]";
    return ss.str();
  }
}

bool FlagBase::ConsumeNamePrefix(FlagParser& parser,
                                 absl::string_view* arg) const {
  absl::string_view input = *arg;
  if (!long_name_.empty()) {
    if (base::ConsumePrefix(&input, long_name_)) {
      if (input.empty() || input[0] == '=') {
        *arg = input;
        return true;
      }
    }
  }
  if (!short_name_.empty()) {
    if (base::ConsumePrefix(&input, short_name_)) {
      if (input.empty() || input[0] == '=') {
        *arg = input;
        return true;
      }
    }
  }
  return false;
}

bool FlagBase::IsSubParser() const { return false; }

FlagParser::FlagParser() = default;

FlagParser::~FlagParser() = default;

void FlagParser::set_program_name(absl::string_view program_name) {
  program_name_ = std::string(program_name);
}

const std::string& FlagParser::program_name() const { return program_name_; }

const std::string& FlagParser::error_message() {
  if (!error_message_.empty()) return error_message_;
  for (auto& flag : flags_) {
    if (flag->IsSubParser() && flag->is_set()) {
      SubParser* sub_parser = reinterpret_cast<SubParser*>(flag.get());
      return sub_parser->error_message();
    }
  }
  return base::EmptyString();
}

FlagBase& FlagParser::AddFlag(std::unique_ptr<FlagBase> flag_base) {
  flags_.push_back(std::move(flag_base));
  return *flags_.back().get();
}

SubParser& FlagParser::AddSubParser() {
  std::unique_ptr<SubParser> sub_parser(new SubParser());
  flags_.push_back(std::move(sub_parser));
  return *reinterpret_cast<SubParser*>(flags_.back().get());
}

bool FlagParser::Validate() {
  bool is_positional = true;
  bool has_subparser = false;
  for (auto& flag : flags_) {
    if (!(flag->is_positional() || flag->is_optional())) {
      error_message_ = "Flag should be positional or optional.";
      return false;
    }

    if (flag->is_positional() && flag->is_optional()) {
      error_message_ = absl::Substitute(
          "\"$0\" is positional and optional, please choose either one of "
          "them.",
          flag->name());
      return false;
    }

    if (flag->IsSubParser()) {
      if (flag->is_optional()) {
        error_message_ = absl::Substitute(
            "Subparser \"$0\" should be positional.", flag->display_name());
        return false;
      }

      has_subparser = true;
      SubParser* sub_parser = reinterpret_cast<SubParser*>(flag.get());
      if (!sub_parser->Validate()) {
        return false;
      }
    } else {
      if (flag->is_positional()) {
        if (!is_positional) {
          error_message_ = absl::Substitute(
              "\"$0\" should be before any optional arguments.",
              flag->display_name());
          return false;
        }
      } else {
        is_positional = false;
      }
    }

    if (!flag->NeedsValue()) {
      if (flag->is_positional()) {
        error_message_ = absl::Substitute(
            "\"$0\" can't parse a value, how about considering using "
            "set_short_name() or set_long_name()?",
            flag->name());
        return false;
      }
    }
  }

  if (has_subparser) {
    for (auto& flag : flags_) {
      if (flag->is_positional()) {
        if (!flag->IsSubParser()) {
          error_message_ = absl::Substitute(
              "\"$0\" can't be positional if the parser has "
              "subparser, how about considering using "
              "set_short_name() or set_long_name()?",
              flag->name());
          return false;
        }
      }
    }
  }

  return true;
}

bool FlagParser::Parse(int argc, char** argv, int from) {
  current_idx_ = from;
  argc_ = argc;
  argv_ = argv;

  if (!Validate()) return false;

  int positional_parsed = 0;
  int positional_argument = std::count_if(
      flags_.begin(), flags_.end(), [](const std::unique_ptr<FlagBase>& flag) {
        return flag->is_positional();
      });
  bool has_subparser = false;
  if (positional_argument > 0) {
    has_subparser = flags_[0]->IsSubParser();
  }

  while (current_idx_ < argc_) {
    absl::string_view arg = current();
    if (arg == "--help" || arg == "-h") {
      std::cerr << help_message() << std::endl;
      error_message_ = absl::Substitute("Got \"$0\".", arg);
      return false;
    }

    FlagBase* target_flag = nullptr;
    if (has_subparser) {
      for (int i = 0; i < positional_argument; ++i) {
        if (flags_[i]->name() == arg) {
          target_flag = flags_[i].get();
          break;
        }
      }
    }

    if (!has_subparser && positional_parsed < positional_argument) {
      int positional_idx = 0;
      for (auto& flag : flags_) {
        if (flag->is_optional())
          continue;
        else {
          if (positional_idx == positional_parsed) {
            target_flag = flag.get();
            break;
          }

          positional_idx++;
        }
      }
    } else {
      for (auto& flag : flags_) {
        if (flag->is_positional()) continue;

        if (!flag->ConsumeNamePrefix(*this, &arg)) continue;
        target_flag = flag.get();
        break;
      }
    }

    bool parsed = false;
    std::string reason;
    if (target_flag) {
      if (target_flag->IsSubParser()) {
        target_flag->is_set_ = true;
        SubParser* sub_parser = reinterpret_cast<SubParser*>(target_flag);
        sub_parser->set_program_name(
            absl::Substitute("$0 $1", program_name_, target_flag->name()));
        return sub_parser->Parse(argc, argv, current_idx_ + 1);
      } else if (target_flag->is_positional()) {
        parsed = target_flag->ParseValue(arg, &reason);
        positional_parsed++;
      } else {
        if (target_flag->NeedsValue() && !ConsumeEqualOrProceed(&arg)) {
          error_message_ = absl::Substitute(
              "\"$0\" is failed to parse: (reason: empty value ).",
              target_flag->display_name());
          return false;
        }
        parsed = target_flag->ParseValue(arg, &reason);
      }
    }

    if (!parsed) {
      if (target_flag) {
        if (reason.empty()) {
          error_message_ =
              absl::Substitute("\"$0\" is failed to parse: (reason: unknown).",
                               target_flag->display_name());
        } else {
          error_message_ =
              absl::Substitute("\"$0\" is failed to parse: (reason: $1).",
                               target_flag->display_name(), reason);
        }
      } else {
        absl::string_view candidate_arg;
        bool found = FindTheMostSimilarFlag(arg, &candidate_arg);
        if (found) {
          error_message_ = absl::Substitute(
              "met unknown argument: \"$0\", maybe you mean \"$1\"?", arg,
              candidate_arg);
        } else {
          error_message_ =
              absl::Substitute("met unknown argument: \"$0\".", arg);
        }
      }
      return false;
    }
    Proceed();
  }

  if (!has_subparser && positional_parsed < positional_argument) {
    for (auto& flag : flags_) {
      if (flag->is_optional()) continue;
      if (!flag->is_set()) {
        error_message_ = absl::Substitute("\"$0\" is positional, but not set.",
                                          flag->name());
        return false;
      }
    }
  }

  for (auto& flag : flags_) {
    if (flag->is_required() && !flag->is_set()) {
      error_message_ = absl::Substitute("\"$0\" is required, but not set.",
                                        flag->display_name());
      return false;
    }
  }

  return true;
}

absl::string_view FlagParser::current() { return argv_[current_idx_]; }

bool FlagParser::ConsumeEqualOrProceed(absl::string_view* arg) {
  if (base::ConsumePrefix(arg, "=")) return true;
  if (!arg->empty()) return false;
  Proceed();
  *arg = current();
  return true;
}

void FlagParser::Proceed() {
  if (current_idx_ < argc_) current_idx_++;
}

bool FlagParser::FindTheMostSimilarFlag(absl::string_view input,
                                        absl::string_view* output) {
  size_t theshold = (input.length() + 1) / 2;
  size_t min = std::numeric_limits<size_t>::max();

  for (auto& flag : flags_) {
    if (!flag->short_name().empty()) {
      size_t dist = GetLevenshteinDistance(input, flag->short_name());
      if (dist <= theshold && dist < min) {
        *output = flag->short_name();
        min = dist;
      }
    }

    if (!flag->long_name().empty()) {
      size_t dist = GetLevenshteinDistance(input, flag->long_name());
      if (dist <= theshold && dist < min) {
        *output = flag->long_name();
        min = dist;
      }
    }
  }

  return min < std::numeric_limits<size_t>::max();
}

std::string FlagParser::help_message() {
  std::stringstream ss;
  ss << "Usage: " << std::endl << std::endl;
  ss << program_name_;
  size_t flag_start = program_name_.length();
  int remain_len = kDefaultLineWidth - flag_start;
  bool has_sub_parser = false;
  for (auto& flag : flags_) {
    if (flag->IsSubParser()) {
      has_sub_parser = true;
      continue;
    }
    if (remain_len < 0) {
      ss << std::endl;
      ss << std::string(flag_start, ' ');
      remain_len = kDefaultLineWidth;
    }
    remain_len -= AppendAndDecreaseLength(ss, " ");
    remain_len -= AppendAndDecreaseLength(ss, flag->display_usage());
  }
  if (has_sub_parser) {
    remain_len -= AppendAndDecreaseLength(ss, " <command> <args>");
  }
  ss << std::endl;

  if (has_sub_parser) {
    ss << std::endl << "Commands:" << std::endl << std::endl;
    for (auto& flag : flags_) {
      if (flag->IsSubParser()) {
        ss << flag->display_help(kDefaultHelpStart) << std::endl;
      }
    }
  }

  bool has_positional_flag = std::any_of(
      flags_.begin(), flags_.end(), [](const std::unique_ptr<FlagBase>& flag) {
        return flag->is_positional() && !flag->IsSubParser();
      });
  if (has_positional_flag) {
    ss << std::endl << "Positional arguments:" << std::endl << std::endl;
    for (auto& flag : flags_) {
      if (flag->is_positional()) {
        ss << flag->display_help(kDefaultHelpStart) << std::endl;
      }
    }
  }

  bool has_optional_flag = std::any_of(
      flags_.begin(), flags_.end(), [](const std::unique_ptr<FlagBase>& flag) {
        return flag->is_optional();
      });
  if (has_optional_flag) {
    ss << std::endl << "Optional arguments:" << std::endl << std::endl;
    for (auto& flag : flags_) {
      if (flag->is_optional()) {
        ss << flag->display_help(kDefaultHelpStart) << std::endl;
      }
    }
  }

  return ss.str();
}

SubParser::SubParser() = default;

SubParser::~SubParser() = default;

bool SubParser::IsSubParser() const { return true; }

bool SubParser::NeedsValue() const { return true; }

bool SubParser::ParseValue(absl::string_view arg, std::string* reason) {
  // Should not be called!!!
  return false;
}

}  // namespace console