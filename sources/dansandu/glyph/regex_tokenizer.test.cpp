#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/regex_tokenizer.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::error::TokenizationError;
using dansandu::glyph::regex_tokenizer::RegexTokenizer;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

// clang-format off
TEST_CASE("RegexTokenizer")
{
    auto identifier = Symbol{0};
    auto number     = Symbol{1};
    auto add        = Symbol{2};
    auto whitespace = Symbol{3};

    auto tokenizer = RegexTokenizer{{{identifier, "[a-zA-Z]\\w*"},
                                     {number,     "([1-9]\\d*|0)(\\.\\d+)?"},
                                     {add,        "\\+"},
                                     {whitespace, "\\s+"}}};

    REQUIRE(tokenizer("") == std::vector<Token>{});

    REQUIRE(tokenizer("a + 10") == std::vector<Token>{{identifier, 0, 1},
                                                      {whitespace, 1, 2},
                                                      {add, 2, 3},
                                                      {whitespace, 3, 4},
                                                      {number, 4, 6}});

    REQUIRE_THROWS_AS(tokenizer("a + & + 20"), TokenizationError);

    REQUIRE_THROWS_AS(tokenizer("a + f()"), TokenizationError);

    REQUIRE_THROWS_AS(tokenizer("@a + 10"), TokenizationError);
}
// clang-format on
