#pragma once

#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/implementation/tokenization.hpp"

#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace dansandu::glyph::implementation::parser {

class SyntaxError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class Node {
    friend bool operator==(const Node& left, const Node& right);
    friend std::ostream& operator<<(std::ostream& stream, const Node& node);

    using Token = dansandu::glyph::implementation::tokenization::Token;

public:
    explicit Node(Token token);

    Node(int ruleIndex, std::vector<Node> children);

    bool isRule() const;

    bool isToken() const;

    int getRuleIndex() const;

    const Token& getToken() const;

    const Node& getChild(int index) const;

private:
    int ruleIndex_;
    Token token_;
    std::vector<Node> children_;
};

bool operator==(const Node& left, const Node& right);

bool operator!=(const Node& left, const Node& right);

std::ostream& operator<<(std::ostream& stream, const Node& node);

Node parse(std::vector<dansandu::glyph::implementation::tokenization::Token> tokens,
           const dansandu::glyph::implementation::parsing_table::ParsingTable& parsingTable,
           const std::vector<dansandu::glyph::implementation::grammar::Rule>& rules);
}
