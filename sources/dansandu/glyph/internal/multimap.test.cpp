#include "dansandu/glyph/internal/multimap.hpp"
#include "catchorg/catch/catch.hpp"

#include <set>

using dansandu::glyph::internal::multimap::Multimap;
using dansandu::glyph::symbol::Symbol;

TEST_CASE("Multimap")
{
    SECTION("set values")
    {
        auto table = Multimap{};

        table[Symbol{0}] = {Symbol{1}, Symbol{2}};

        REQUIRE(table[Symbol{0}] == std::vector<Symbol>{Symbol{1}, Symbol{2}});

        table[Symbol{10}] = {Symbol{13}, Symbol{15}};

        REQUIRE(table[Symbol{0}] == std::vector<Symbol>{Symbol{1}, Symbol{2}});

        REQUIRE(table[Symbol{10}] == std::vector<Symbol>{Symbol{13}, Symbol{15}});

        table[Symbol{20}] = {Symbol{26}, Symbol{29}, Symbol{21}};

        REQUIRE(table[Symbol{0}] == std::vector<Symbol>{Symbol{1}, Symbol{2}});

        REQUIRE(table[Symbol{10}] == std::vector<Symbol>{Symbol{13}, Symbol{15}});

        REQUIRE(table[Symbol{20}] == std::vector<Symbol>{Symbol{26}, Symbol{29}, Symbol{21}});

        SECTION("merge with new symbol")
        {
            table.merge({Symbol{0}, Symbol{40}});

            REQUIRE(table[Symbol{0}] == std::vector<Symbol>{Symbol{1}, Symbol{2}});

            REQUIRE(table[Symbol{40}] == table[Symbol{0}]);

            REQUIRE(table[Symbol{10}] == std::vector<Symbol>{Symbol{13}, Symbol{15}});

            REQUIRE(table[Symbol{20}] == std::vector<Symbol>{Symbol{26}, Symbol{29}, Symbol{21}});
        }

        SECTION("merge with existing symbols")
        {
            table.merge({Symbol{0}, Symbol{20}});

            REQUIRE(table[Symbol{0}] == std::vector<Symbol>{Symbol{1}, Symbol{2}, Symbol{26}, Symbol{29}, Symbol{21}});

            REQUIRE(table[Symbol{0}] == table[Symbol{20}]);

            REQUIRE(table[Symbol{10}] == std::vector<Symbol>{Symbol{13}, Symbol{15}});
        }

        SECTION("iteration")
        {
            auto actualPartitions = std::vector<std::vector<Symbol>>{};
            auto actualValues = std::vector<std::vector<Symbol>>{};

            table.forEach(
                [&](const auto& p, const auto& v)
                {
                    actualPartitions.push_back(p);
                    actualValues.push_back(v);
                });

            const auto expectedPartitions = std::vector<std::vector<Symbol>>{{Symbol{0}}, {Symbol{10}}, {Symbol{20}}};

            const auto expectedValues = std::vector<std::vector<Symbol>>{
                {Symbol{1}, Symbol{2}}, {Symbol{13}, Symbol{15}}, {Symbol{26}, Symbol{29}, Symbol{21}}};

            REQUIRE(actualPartitions == expectedPartitions);

            REQUIRE(actualValues == expectedValues);
        }
    }
}
