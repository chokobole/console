#include "console/flag.h"
#include "console/stream.h"

int main(int argc, char** argv) {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  uint16_t number;
  console::FlagParser flag_parser;
  flag_parser.set_program_name("custom_flag");
  flag_parser
      .AddFlag<console::Uint16Flag>(
          [&number](absl::string_view input, std::string* reason) {
            uint16_t n;
            if (!console::FlagValueTraits<uint16_t>::ParseValue(input, &n,
                                                                reason)) {
              return false;
            }
            if (n % 2 == 0) {
              *reason = absl::StrFormat("%u is not a odd number", n);
              return false;
            }
            number = n;
            return true;
          })
      .set_name("number")
      .set_required()
      .set_help("Please input only odd numbers!");
  if (!flag_parser.Parse(argc, argv)) {
    {
      console::Stream stream(std::cerr);
      stream.Red();
      std::cerr << "[ERROR]: ";
    }
    std::cerr << flag_parser.error_message() << std::endl;
    return 1;
  }

  return 0;
}