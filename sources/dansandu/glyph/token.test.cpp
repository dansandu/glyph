#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::token::Token;

TEST_CASE("Token") {
    auto token = Token{"identifier", 2, 3};

    REQUIRE(token.getIdentifier() == "identifier");

    REQUIRE(token.begin() == 2);

    REQUIRE(token.end() == 3);
}
