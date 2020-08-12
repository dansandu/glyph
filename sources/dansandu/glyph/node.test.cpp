#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/node.hpp"
#include "dansandu/glyph/symbol.hpp"
#include "dansandu/glyph/token.hpp"

using dansandu::glyph::node::Node;
using dansandu::glyph::symbol::Symbol;
using dansandu::glyph::token::Token;

TEST_CASE("Node")
{
    SECTION("with token")
    {
        auto token = Token{Symbol{1}, 0, 1};

        auto node = Node{token};

        REQUIRE(node.isToken());

        REQUIRE(!node.isRule());

        REQUIRE(node.getToken() == token);

        REQUIRE_THROWS_AS(node.getRuleIndex(), std::runtime_error);
    }

    SECTION("with production rule")
    {
        constexpr auto ruleIndex = 23;
        auto node = Node{ruleIndex};

        REQUIRE(node.isRule());

        REQUIRE(!node.isToken());

        REQUIRE(node.getRuleIndex() == ruleIndex);

        REQUIRE_THROWS_AS(node.getToken(), std::runtime_error);
    }
}
