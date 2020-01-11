// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_AUTOCOMPLETION_H_
#define CONSOLE_AUTOCOMPLETION_H_

#include "console/export.h"
#include "console/flag.h"

namespace console {

class CONSOLE_EXPORT Autocompletion {
 public:
  static bool WriteToJson(const FlagParser& flag_parser,
                          absl::string_view filename);

 private:
  static void WriteFlagParserToJson(std::ofstream& ofs, int depth,
                                    const FlagParser* flag_parser);
  static void WriteFlagToJson(std::ofstream& ofs, int depth,
                              const FlagBase* flag);
};

}  // namespace console

#endif  // CONSOLE_AUTOCOMPLETION_H_