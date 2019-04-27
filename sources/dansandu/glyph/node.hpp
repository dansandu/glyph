#pragma once

#include "dansandu/glyph/token.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::node {

class Node {
    friend bool operator==(const Node& left, const Node& right);

    friend std::ostream& operator<<(std::ostream& stream, const Node& node);

public:
    explicit Node(dansandu::glyph::token::Token token);

    Node(int ruleIndex, std::vector<Node> children);

    bool isRule() const;

    bool isToken() const;

    int getRuleIndex() const;

    const dansandu::glyph::token::Token& getToken() const;

    const Node& getChild(int index) const;

    int getChildrenCount() const;

private:
    int ruleIndex_;
    dansandu::glyph::token::Token token_;
    std::vector<Node> children_;
};

bool operator==(const Node& left, const Node& right);

bool operator!=(const Node& left, const Node& right);

std::ostream& operator<<(std::ostream& stream, const Node& node);

}
