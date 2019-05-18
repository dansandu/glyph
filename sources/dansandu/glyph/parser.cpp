#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/implementation/grammar.hpp"
#include "dansandu/glyph/implementation/parser.hpp"
#include "dansandu/glyph/implementation/parsing_table.hpp"
#include "dansandu/glyph/implementation/tokenization.hpp"

#include <string>
#include <string_view>
#include <vector>

using dansandu::glyph::implementation::automaton::getAutomaton;
using dansandu::glyph::implementation::grammar::getFirstTable;
using dansandu::glyph::implementation::grammar::getFollowTable;
using dansandu::glyph::implementation::grammar::getRules;
using dansandu::glyph::implementation::grammar::getSymbols;
using dansandu::glyph::implementation::parsing_table::getSimpleLeftToRightParsingTable;
using dansandu::glyph::implementation::tokenization::tokenize;
using dansandu::glyph::node::Node;
using dansandu::glyph::token::TokenDescriptor;

namespace dansandu::glyph::parser {

Parser::Parser(std::string_view grammar, std::vector<TokenDescriptor> tokenDescriptors)
    : tokenDescriptors_{std::move(tokenDescriptors)}, rules_{getRules(grammar)} {
    auto automaton = getAutomaton(rules_);
    auto symbols = getSymbols(rules_);
    auto firstTable = getFirstTable(rules_, symbols);
    auto followTable = getFollowTable(rules_, firstTable);
    parsingTable_ = getSimpleLeftToRightParsingTable(rules_, automaton, followTable, symbols.first);
}

Node Parser::parse(std::string_view string, const std::vector<std::string>& discard) const {
    auto tokens = tokenize(string, tokenDescriptors_, discard);
    return dansandu::glyph::implementation::parser::parse(tokens, parsingTable_, rules_);
}

}
