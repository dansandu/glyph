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

    auto descriptors = std::vector<RegexTokenizer::Descriptor>{{identifier, std::regex{"[a-zA-Z]\\w*"}},
                                                               {number, std::regex{"([1-9]\\d*|0)(\\.\\d+)?"}},
                                                               {add, std::regex{"\\+"}},
                                                               {whitespace, std::regex{"\\s+"}}};

    SECTION("without discard")
    {
        auto tokenizer = RegexTokenizer{descriptors};

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

    SECTION("with discard")
    {
        auto tokenizer = RegexTokenizer{descriptors, {whitespace}};

        REQUIRE(tokenizer("a   + 1000") == std::vector<Token>{{identifier, 0, 1},
                                                              {add, 4, 5},
                                                              {number, 6, 10}});
    }
}
// clang-format on
