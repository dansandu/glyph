#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/parser.hpp"
#include "dansandu/glyph/token.hpp"

using Catch::Detail::Approx;
using dansandu::glyph::node::Node;
using dansandu::glyph::parser::Parser;
using dansandu::glyph::token::RegexTokenizer;

static double evaluate(std::string_view formula, const Node& node) {
    if (node.isToken())
        throw std::runtime_error{"oopsie -- ill-formed syntax tree, node should always be a production rule!"};
    auto ruleIndex = node.getRuleIndex();
    if (ruleIndex == 0 || ruleIndex == 2 || ruleIndex == 4) {
        return evaluate(formula, node.getChild(0));
    } else if (ruleIndex == 1) {
        return evaluate(formula, node.getChild(0)) + evaluate(formula, node.getChild(2));
    } else if (ruleIndex == 3) {
        return evaluate(formula, node.getChild(0)) * evaluate(formula, node.getChild(2));
    } else if (ruleIndex == 5) {
        const auto& token = node.getChild(0).getToken();
        return std::stod(std::string{formula.cbegin() + token.begin(), formula.cbegin() + token.end()});
    } else {
        throw std::runtime_error{"oopsie -- unrecognized production rule index!"};
    }
}

// clang-format off
TEST_CASE("Parser") {
    auto tokenizer = RegexTokenizer{{{"identifier", "[a-z]\\w*"},
                                     {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                     {"add", "\\+"},
                                     {"multiply", "\\*"},
                                     {"whitespace", "\\s+"}}, {"whitespace"}};
    auto parser = Parser{/*0*/"Start    -> Sums                    \n"
                         /*1*/"Sums     -> Sums add Products       \n"
                         /*2*/"Sums     -> Products                \n"
                         /*3*/"Products -> Products multiply Value \n"
                         /*4*/"Products -> Value                   \n"
                         /*5*/"Value    -> number"};
    auto formula = "12.39 + 5 * 7 + 3 * 2.0 + 1000";
    auto tree = parser.parse(formula, tokenizer);

    REQUIRE(evaluate(formula, tree) == Approx(1053.39));
}
// clang-format on
