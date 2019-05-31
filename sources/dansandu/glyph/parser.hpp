#pragma once

#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parser.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/node.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace dansandu::glyph::parser {

class Parser {
public:
    explicit Parser(std::string grammar);

    template<typename Tokenizer>
    dansandu::glyph::node::Node parse(std::string_view string, Tokenizer&& tokenizer) const {
        return dansandu::glyph::implementation::parser::parse(tokenizer(string), parsingTable_, grammar_.getRules());
    }

private:
    dansandu::glyph::implementation::grammar::Grammar grammar_;
    dansandu::glyph::implementation::parsing_table::ParsingTable parsingTable_;
};

}
