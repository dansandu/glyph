#include "dansandu/ballotin/container.hpp"
#include "dansandu/glyph/implementation/tokenization.hpp"

#include <string>
#include <utility>
#include <vector>

using dansandu::ballotin::container::operator<<;
using dansandu::glyph::implementation::tokenization::operator<<;

#include "catchorg/catch/catch.hpp"

using dansandu::glyph::implementation::tokenization::getTerminalsPattern;
using dansandu::glyph::implementation::tokenization::Token;
using dansandu::glyph::implementation::tokenization::TokenizationError;
using dansandu::glyph::implementation::tokenization::tokenize;

// clang-format off
TEST_CASE("Tokenization") {
    const auto terminals = std::vector<std::pair<std::string, std::string>>{{{"identifier", "[a-z]\\w*"},
                                                                             {"number", "(?:[1-9]\\d*|0)(?:\\.\\d+)?"},
                                                                             {"add", "\\+"},
                                                                             {"whitespace", "\\s+"}}};

    SECTION("terminals pattern") {
        REQUIRE(getTerminalsPattern(terminals) == "([a-z]\\w*)|((?:[1-9]\\d*|0)(?:\\.\\d+)?)|(\\+)|(\\s+)");
    }

    SECTION("tokenize") {
        REQUIRE(tokenize("", terminals) == std::vector<Token>{});

        REQUIRE(tokenize("a + 10", terminals) == std::vector<Token>{{{"identifier", 0, 1},
                                                                     {"whitespace", 1, 2},
                                                                     {"add", 2, 3},
                                                                     {"whitespace", 3, 4},
                                                                     {"number", 4, 6}}});

        REQUIRE_THROWS_AS(tokenize("a + & + 20", terminals), TokenizationError);

        REQUIRE_THROWS_AS(tokenize("a + f()", terminals), TokenizationError);

        REQUIRE_THROWS_AS(tokenize("@a + 10", terminals), TokenizationError);

        SECTION("with discard") {
            REQUIRE(tokenize("a   + 1000", terminals, {"whitespace"}) == std::vector<Token>{{{"identifier", 0, 1},
                                                                                             {"add", 4, 5},
                                                                                             {"number", 6, 10}}});
        }
    }
}
// clang-format on
