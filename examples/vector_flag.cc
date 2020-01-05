#include <iostream>

#include "console/flag.h"
#include "console/stream.h"

int main(int argc, char** argv) {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  std::vector<int> numbers;
  console::FlagParser flag_parser;
  flag_parser.set_program_name("vector_flag");
  flag_parser.AddFlag<console::Flag<std::vector<int>>>(&numbers)
      .set_short_name("-n")
      .set_long_name("--number")
      .set_help("numbers for add, you can accumulate!")
      .set_required();
  if (!flag_parser.Parse(argc, argv)) {
    {
      console::Stream stream(std::cerr);
      stream.Red();
      std::cerr << "[ERROR]: ";
    }
    std::cerr << flag_parser.error_message() << std::endl;
    return 1;
  }

  int ret = 0;
  for (int number : numbers) {
    ret += number;
  }
  {
    console::Stream stream;
    stream.Green();
    std::cout << "[SUM]: ";
  }
  std::cout << ret << std::endl;

  return 0;
}