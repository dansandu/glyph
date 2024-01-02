#pragma once

#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/tokenizer.hpp"

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

    std::vector<dansandu::glyph::node::Node> parse(const std::string_view text,
                                                   const dansandu::glyph::tokenizer::ITokenizer& tokenizer) const;

    void print(std::ostream& stream) const;

private:
    std::shared_ptr<const void> implementation_;
};

}
