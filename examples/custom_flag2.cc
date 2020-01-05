#include "console/flag.h"
#include "console/stream.h"

struct OddNumber {
  uint16_t number;
};

namespace console {

template <>
class FlagValueTraits<OddNumber> {
 public:
  static bool ParseValue(absl::string_view input, OddNumber* value,
                         std::string* reason) {
    uint32_t n;
    if (absl::SimpleAtoi(input, &n)) {
      if (n > std::numeric_limits<uint16_t>::max()) {
        *reason = absl::StrFormat("%u is out of its range", n);
        return false;
      }
      if (n % 2 == 0) {
        *reason = absl::StrFormat("%u is not a odd number", n);
        return false;
      } else {
        value->number = n;
        return true;
      }
    }

    *reason = absl::StrFormat("failed to convert to number (\"%s\")", input);
    return false;
  }
};

}  // namespace console

int main(int argc, char** argv) {
#if defined(OS_WIN)
  console::Console::EnableAnsi(std::cout);
#endif

  OddNumber number;
  console::FlagParser flag_parser;
  flag_parser.set_program_name("custom_flag2");
  flag_parser.AddFlag<console::Flag<OddNumber>>(&number)
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