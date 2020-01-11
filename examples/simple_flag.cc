// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include "console/autocompletion.h"
#include "console/flag.h"
#include "console/stream.h"

int main(int argc, char** argv) {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  struct Person {
    std::string name;
    uint8_t age;
    bool married;

    void Print() {
      std::cout << "{" << std::endl;
      std::cout << "  name: " << name << std::endl;
      std::cout << "  age: " << static_cast<int>(age) << std::endl;
      std::cout << "  married: " << std::boolalpha << married << std::endl;
      std::cout << "}" << std::endl;
    }
  };

  Person person;
  console::FlagParser flag_parser;
  flag_parser.set_program_name("simple_flag");
  flag_parser.AddFlag<console::StringFlag>(&person.name)
      .set_name("name")
      .set_help("what's your name?");
  flag_parser.AddFlag<console::Uint8Flag>(&person.age)
      .set_name("age")
      .set_help("how old are you?");
  flag_parser.AddFlag<console::BoolFlag>(&person.married)
      .set_long_name("--married")
      .set_help("are you married?");

  // Uncomment if you want to generate json file used by console-autocomplete!
  // console::Autocompletion::WriteToJson(flag_parser, "simple_flag.json");

  if (!flag_parser.Parse(argc, argv)) {
    {
      console::Stream stream(std::cerr);
      stream.Red();
      std::cerr << "[ERROR]: ";
    }
    std::cerr << flag_parser.error_message() << std::endl;
    return 1;
  }
  person.Print();

  return 0;
}