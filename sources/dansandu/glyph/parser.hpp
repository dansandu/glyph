#pragma once

#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <memory>
#include <ostream>
#include <string_view>
#include <vector>

namespace dansandu::glyph::parser
{

class PRALINE_EXPORT Parser
{
public:
    explicit Parser(const std::string_view grammar);

    dansandu::glyph::symbol::Symbol getTerminalSymbol(const std::string_view identifier) const;

    dansandu::glyph::symbol::Symbol getDiscardedSymbolPlaceholder() const;

    void parse(const std::vector<dansandu::glyph::token::Token>& tokens,
               const std::function<void(const dansandu::glyph::node::Node&)>& visitor) const;

    void dump(std::ostream& stream) const;

private:
    std::shared_ptr<const void> implementation_;
};

}
