#include "dansandu/glyph/regex_tokenizer.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::error::TokenizationError;
using dansandu::glyph::regex_tokenizer::RegexTokenizer;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

TEST_CASE("RegexTokenizer")
{
    const auto identifier = Symbol{0};
    const auto number = Symbol{1};
    const auto add = Symbol{2};
    const auto whitespace = Symbol{3};

    const auto tokenizer = RegexTokenizer{
        {{identifier, "[a-zA-Z]\\w*"}, {number, "([1-9]\\d*|0)(\\.\\d+)?"}, {add, "\\+"}, {whitespace, "\\s+"}}};

    SECTION("empty text")
    {
        const auto expected = std::vector<Token>{};

        REQUIRE(tokenizer.tokenize("") == expected);
    }

    SECTION("good text")
    {
        const auto expected =
            std::vector<Token>{{identifier, 0, 1}, {whitespace, 1, 2}, {add, 2, 3}, {whitespace, 3, 4}, {number, 4, 6}};

        REQUIRE(tokenizer.tokenize("a + 10") == expected);
    }

    SECTION("bad text")
    {
        REQUIRE_THROWS_AS(tokenizer.tokenize("a + & + 20"), TokenizationError);

        REQUIRE_THROWS_AS(tokenizer.tokenize("a + f()"), TokenizationError);

        REQUIRE_THROWS_AS(tokenizer.tokenize("@a + 10"), TokenizationError);
    }
}
