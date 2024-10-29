#include "dansandu/glyph/internal/multimap.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <set>

using dansandu::glyph::internal::multimap::Multimap;
using dansandu::glyph::symbol::Symbol;

using Symbols = std::vector<Symbol>;

template<typename... T>
Symbols symbols(T... arguments)
{
    return {Symbol{arguments}...};
}

TEST_CASE("Multimap")
{
    auto table = Multimap{};

    const auto subscriptAt = [&](const int id) -> auto& { return table[Symbol{id}]; };

    subscriptAt(0) = symbols(1, 2);

    REQUIRE(subscriptAt(0) == symbols(1, 2));

    subscriptAt(10) = symbols(13, 15);

    REQUIRE(subscriptAt(0) == symbols(1, 2));

    REQUIRE(subscriptAt(10) == symbols(13, 15));

    subscriptAt(20) = symbols(26, 29, 21);

    REQUIRE(subscriptAt(0) == symbols(1, 2));

    REQUIRE(subscriptAt(10) == symbols(13, 15));

    REQUIRE(subscriptAt(20) == symbols(26, 29, 21));

    SECTION("merge with new symbol")
    {
        table.merge(symbols(0, 40));

        REQUIRE(subscriptAt(0) == symbols(1, 2));

        REQUIRE(subscriptAt(40) == subscriptAt(0));

        REQUIRE(subscriptAt(10) == symbols(13, 15));

        REQUIRE(subscriptAt(20) == symbols(26, 29, 21));
    }

    SECTION("merge with existing symbols")
    {
        table.merge(symbols(0, 20));

        REQUIRE(subscriptAt(0) == symbols(1, 2, 26, 29, 21));

        REQUIRE(subscriptAt(0) == subscriptAt(20));

        REQUIRE(subscriptAt(10) == symbols(13, 15));
    }

    SECTION("iteration")
    {
        auto actualPartitions = std::vector<Symbols>{};
        auto actualValues = std::vector<Symbols>{};

        table.forEach(
            [&](const auto& p, const auto& v)
            {
                actualPartitions.push_back(p);
                actualValues.push_back(v);
            });

        const auto expectedPartitions = std::vector<Symbols>{symbols(0), symbols(10), symbols(20)};

        const auto expectedValues = std::vector<Symbols>{symbols(1, 2), symbols(13, 15), symbols(26, 29, 21)};

        REQUIRE(actualPartitions == expectedPartitions);

        REQUIRE(actualValues == expectedValues);
    }
}
