#pragma once

#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/token.hpp"

#include <memory>
#include <string>
#include <vector>

namespace dansandu::glyph::parser
{

struct ParserImplementation;

class PRALINE_EXPORT Parser
{
public:
    explicit Parser(const std::string& grammar);

    ~Parser();

    dansandu::glyph::node::Node parse(const std::vector<dansandu::glyph::token::Token>& tokens) const;

private:
    std::unique_ptr<ParserImplementation> implementation_;
};

}
