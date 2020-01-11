// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>
#include <iostream>

#include "console/autocompletion.h"
#include "console/flag.h"
#include "console/stream.h"

struct Add {
  int a;
  int b;
};

struct Pow {
  int base;
  int exponent;
};

void DoAdd(const Add& add, bool verbose) {
  {
    console::Stream stream;
    stream.Green();
    std::cout << "[ADD]: ";
  }
  if (verbose) {
    std::cout << add.a << " + " << add.b << " = ";
  }
  std::cout << add.a + add.b << std::endl;
}

void DoPow(const Pow& pow, bool verbose) {
  {
    console::Stream stream;
    stream.Green();
    std::cout << "[POW]: ";
  }
  if (verbose) {
    std::cout << pow.base << " ^ " << pow.exponent << " = ";
  }
  std::cout << std::pow(pow.base, pow.exponent) << std::endl;
}

int main(int argc, char** argv) {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  Add add;
  Pow pow;
  console::FlagParser flag_parser;
  flag_parser.set_program_name("sub_parser");
  console::SubParser& add_parser = flag_parser.AddSubParser();
  add_parser.set_name("add").set_help("add");
  add_parser.AddFlag<console::Int32Flag>(&add.a).set_name("a").set_help(
      "number1 for add");
  add_parser.AddFlag<console::Int32Flag>(&add.b).set_name("b").set_help(
      "number2 for add");
  console::SubParser& pow_parser = flag_parser.AddSubParser();
  pow_parser.set_name("pow").set_help("pow");
  pow_parser.AddFlag<console::Int32Flag>(&pow.base).set_name("base").set_help(
      "base for pow");
  pow_parser.AddFlag<console::Int32Flag>(&pow.exponent)
      .set_name("exponent")
      .set_help("exponent for pow");
  bool verbose;
  flag_parser.AddFlag<console::BoolFlag>(&verbose)
      .set_long_name("--verbose")
      .set_help("verbose!");

  // Uncomment if you want to generate json file used by console-autocomplete!
  // console::Autocompletion::WriteToJson(flag_parser, "sub_parser.json");

  if (!flag_parser.Parse(argc, argv)) {
    {
      console::Stream stream(std::cerr);
      stream.Red();
      std::cerr << "[ERROR]: ";
    }
    std::cerr << flag_parser.error_message() << std::endl;
    return 1;
  }

  if (add_parser.is_set()) {
    DoAdd(add, verbose);
  } else if (pow_parser.is_set()) {
    DoPow(pow, verbose);
  } else {
    std::cerr << flag_parser.help_message() << std::endl;
    return 1;
  }

  return 0;
}