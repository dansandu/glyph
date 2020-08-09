#pragma once

#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace dansandu::glyph::parser
{

struct ParserImplementation;

class PRALINE_EXPORT Parser
{
public:
    explicit Parser(std::string_view grammar);

    ~Parser();

    dansandu::glyph::symbol::Symbol getSymbol(std::string_view identifier) const;

    std::vector<dansandu::glyph::node::Node> parse(const std::vector<dansandu::glyph::token::Token>& tokens) const;

private:
    std::unique_ptr<ParserImplementation> implementation_;
};

}
