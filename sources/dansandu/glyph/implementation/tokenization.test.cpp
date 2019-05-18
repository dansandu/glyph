#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/tokenization.hpp"
#include "dansandu/glyph/token.hpp"

#include <string>
#include <utility>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::token::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::implementation::tokenization::getTerminalsPattern;
using dansandu::glyph::implementation::tokenization::TokenizationError;
using dansandu::glyph::implementation::tokenization::tokenize;
using dansandu::glyph::token::Token;
using dansandu::glyph::token::TokenDescriptor;

// clang-format off
TEST_CASE("Tokenization") {
    const auto tokenDescriptors = std::vector<TokenDescriptor>{{{"identifier", "[a-z]\\w*"},
                                                                {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                                                {"add", "\\+"},
                                                                {"whitespace", "\\s+"}}};

    SECTION("terminals pattern") {
        REQUIRE(getTerminalsPattern(tokenDescriptors) == "([a-z]\\w*)|((?:[1-9]\\d*|0)(?:\\.\\d+)?)|(\\+)|(\\s+)");
    }

    SECTION("tokenize") {
        REQUIRE(tokenize("", tokenDescriptors) == std::vector<Token>{});

        REQUIRE(tokenize("a + 10", tokenDescriptors) == std::vector<Token>{{{"identifier", 0, 1},
                                                                            {"whitespace", 1, 2},
                                                                            {"add", 2, 3},
                                                                            {"whitespace", 3, 4},
                                                                            {"number", 4, 6}}});

        REQUIRE_THROWS_AS(tokenize("a + & + 20", tokenDescriptors), TokenizationError);

        REQUIRE_THROWS_AS(tokenize("a + f()", tokenDescriptors), TokenizationError);

        REQUIRE_THROWS_AS(tokenize("@a + 10", tokenDescriptors), TokenizationError);

        SECTION("with discard") {
            REQUIRE(tokenize("a   + 1000", tokenDescriptors, {"whitespace"}) == std::vector<Token>{{{"identifier", 0, 1},
                                                                                                    {"add", 4, 5},
                                                                                                    {"number", 6, 10}}});
        }
    }
}
// clang-format on
