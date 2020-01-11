#include "console/autocompletion.h"

#include <fstream>

#include "absl/strings/substitute.h"

namespace console {

namespace {

void WriteIndent(std::ofstream& ofs, int depth) {
  for (int i = 0; i < depth * 2; ++i) {
    ofs << ' ';
  }
}

}  // namespace

// static
bool Autocompletion::WriteToJson(const FlagParser& flag_parser,
                                 absl::string_view filename) {
  std::ofstream ofs(filename.data());
  if (!ofs.good()) return false;

  WriteFlagParserToJson(ofs, 0, &flag_parser);

  return true;
}

// static
void Autocompletion::WriteFlagParserToJson(std::ofstream& ofs, int depth,
                                           const FlagParser* flag_parser) {
  WriteIndent(ofs, depth);
  ofs << "[\n";
  for (size_t i = 0; i < flag_parser->flags_.size(); ++i) {
    WriteFlagToJson(ofs, depth + 1, flag_parser->flags_[i].get());
    if (i != flag_parser->flags_.size() - 1) {
      ofs << ",\n";
    } else {
      ofs << "\n";
    }
  }
  WriteIndent(ofs, depth);
  ofs << "]\n";
}

// static
void Autocompletion::WriteFlagToJson(std::ofstream& ofs, int depth,
                                     const FlagBase* flag) {
  std::vector<std::string> elements;
  if (flag->is_positional()) {
    elements.push_back(absl::Substitute("\"name\": \"$0\"", flag->name()));
  } else {
    if (!flag->short_name().empty()) {
      elements.push_back(
          absl::Substitute("\"shortName\": \"$0\"", flag->short_name()));
    }

    if (!flag->long_name().empty()) {
      elements.push_back(
          absl::Substitute("\"longName\": \"$0\"", flag->long_name()));
    }

    elements.push_back(
        absl::Substitute("\"isSequential\": $0", flag->is_sequential()));
    elements.push_back(
        absl::Substitute("\"needsValue\": $0", flag->NeedsValue()));
  }

  if (!flag->help().empty()) {
    elements.push_back(absl::Substitute("\"help\": \"$0\"", flag->help()));
  }

  if (flag->IsSubParser()) {
    elements.push_back("\"subFlags\": ");
  }

  WriteIndent(ofs, depth);
  ofs << "{\n";
  for (size_t i = 0; i < elements.size(); ++i) {
    WriteIndent(ofs, depth + 1);
    ofs << elements[i];
    if (i != elements.size() - 1) {
      ofs << ",\n";
    } else {
      ofs << "\n";
    }
  }

  if (flag->IsSubParser()) {
    const SubParser* sub_parser = reinterpret_cast<const SubParser*>(flag);
    WriteFlagParserToJson(ofs, depth + 1, sub_parser);
  }

  WriteIndent(ofs, depth);
  ofs << "}";
}

}  // namespace console