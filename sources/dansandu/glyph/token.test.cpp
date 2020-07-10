#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/error.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::error::TokenizationError;
using dansandu::glyph::token::RegexTokenizer;
using dansandu::glyph::token::Token;

// clang-format off
TEST_CASE("Token") {
    auto token = Token{"identifier", 2, 3};

    REQUIRE(token.getIdentifier() == "identifier");

    REQUIRE(token.begin() == 2);

    REQUIRE(token.end() == 3);
}

TEST_CASE("RegexTokenizer") {
    auto descriptors = std::vector<RegexTokenizer::Descriptor>{{{"identifier", "[a-z]\\w*"},
                                                                {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                                                {"add", "\\+"},
                                                                {"whitespace", "\\s+"}}};

    SECTION("without discard") {
        auto tokenizer = RegexTokenizer{descriptors};

        REQUIRE(tokenizer("") == std::vector<Token>{});

        REQUIRE(tokenizer("a + 10") == std::vector<Token>{{{"identifier", 0, 1},
                                                           {"whitespace", 1, 2},
                                                           {"add", 2, 3},
                                                           {"whitespace", 3, 4},
                                                           {"number", 4, 6}}});

        REQUIRE_THROWS_AS(tokenizer("a + & + 20"), TokenizationError);

        REQUIRE_THROWS_AS(tokenizer("a + f()"), TokenizationError);

        REQUIRE_THROWS_AS(tokenizer("@a + 10"), TokenizationError);
    }

    SECTION("with discard") {
        auto tokenizer = RegexTokenizer{descriptors, {"whitespace"}};

        REQUIRE(tokenizer("a   + 1000") == std::vector<Token>{{{"identifier", 0, 1},
                                                               {"add", 4, 5},
                                                               {"number", 6, 10}}});
    }
}
// clang-format on
