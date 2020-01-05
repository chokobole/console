#include <cmath>

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

int DoAdd(const Add& add) { return add.a + add.b; }

double DoPow(const Pow& pow) { return std::pow(pow.base, pow.exponent); }

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
  console::SubParser& sub_parser = flag_parser.AddSubParser();
  sub_parser.set_name("pow").set_help("pow");
  sub_parser.AddFlag<console::Int32Flag>(&pow.base).set_name("base").set_help(
      "base for pow");
  sub_parser.AddFlag<console::Int32Flag>(&pow.exponent)
      .set_name("exponent")
      .set_help("exponent for pow");
  bool verbose;
  flag_parser.AddFlag<console::BoolFlag>(&verbose)
      .set_long_name("--verbose")
      .set_help("verbose!");

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
    {
      console::Stream stream;
      stream.Green();
      std::cout << "[ADD]: ";
    }
    std::cout << DoAdd(add) << std::endl;
  } else {
    {
      console::Stream stream;
      stream.Green();
      std::cout << "[POW]: ";
    }
    std::cout << DoPow(pow) << std::endl;
  }

  return 0;
}