#pragma once

#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parser.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace dansandu::glyph::parser {

class Parser {
public:
    using Node = dansandu::glyph::implementation::parser::Node;

    Parser(std::string_view grammar, std::vector<std::pair<std::string, std::string>> terminals);

    Node parse(std::string_view string, const std::vector<std::string>& discard = {}) const;

private:
    std::vector<std::pair<std::string, std::string>> terminals_;
    std::vector<dansandu::glyph::implementation::grammar::Rule> rules_;
    dansandu::glyph::implementation::parsing_table::ParsingTable parsingTable_;
};

}
