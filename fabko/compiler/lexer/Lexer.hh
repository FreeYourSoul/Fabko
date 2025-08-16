// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 12.08.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef FABKO_LEXER_HH
#define FABKO_LEXER_HH

#include <filesystem>
#include <vector>

namespace fabko::compiler::lexer {

class Lexer {
  public:
    void add_files(std::vector<std::filesystem::path> files);

  private:
    std::vector<std::filesystem::path> files_; //!<
};

} // namespace fabko::compiler::lexer
#endif // FABKO_LEXER_HH
