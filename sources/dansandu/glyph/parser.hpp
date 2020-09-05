#pragma once

#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

#include <functional>
#include <ostream>
#include <string_view>
#include <vector>

namespace dansandu::glyph::parser
{

class PRALINE_EXPORT Parser
{
public:
    explicit Parser(const std::string_view grammar);

    Parser(const Parser&) = delete;

    Parser(Parser&& other) noexcept;

    Parser& operator=(const Parser&) = delete;

    Parser& operator=(Parser&& other) noexcept;

    ~Parser() noexcept;

    dansandu::glyph::symbol::Symbol getTerminalSymbol(const std::string_view identifier) const;

    dansandu::glyph::symbol::Symbol getDiscardedSymbolPlaceholder() const;

    void parse(const std::vector<dansandu::glyph::token::Token>& tokens,
               const std::function<void(const dansandu::glyph::node::Node&)>& visitor) const;

    void dump(std::ostream& stream) const;

private:
    const void* implementation_;
};

}
