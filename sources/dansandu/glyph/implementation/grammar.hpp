#pragma once
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::grammar {

using SymbolTable = std::map<std::string, std::vector<std::string>>;

constexpr auto startSymbol = "Start";

constexpr auto endOfString = "$";

class GrammarError : std::runtime_error {
    using runtime_error::runtime_error;
};

struct Rule {
    std::string leftSide;
    std::vector<std::string> rightSide;
};

bool operator==(const Rule& left, const Rule& right);

bool operator!=(const Rule& left, const Rule& right);

std::ostream& operator<<(std::ostream& stream, const Rule& rule);

std::vector<Rule> getRules(std::string_view grammar);

std::pair<std::vector<std::string>, std::vector<std::string>> getSymbols(const std::vector<Rule>& rules);

SymbolTable getFirstTable(std::vector<Rule> rules,
                          const std::pair<std::vector<std::string>, std::vector<std::string>>& symbols);

SymbolTable getFollowTable(const std::vector<Rule>& rules, const SymbolTable& firstTable);

}
