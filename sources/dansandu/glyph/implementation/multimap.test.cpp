#include "catchorg/catch/catch.hpp"
#include "dansandu/glyph/implementation/multimap.hpp"

#include <set>

using dansandu::glyph::implementation::multimap::Multimap;
using dansandu::glyph::implementation::symbol::Symbol;

TEST_CASE("Multimap")
{
    SECTION("set values")
    {
        auto table = Multimap{};

        table[Symbol{0}] = {Symbol{1}, Symbol{2}};

        REQUIRE(table[Symbol{0}] == std::set<Symbol>{Symbol{1}, Symbol{2}});

        table[Symbol{10}] = {Symbol{13}, Symbol{15}};

        REQUIRE(table[Symbol{0}] == std::set<Symbol>{Symbol{1}, Symbol{2}});

        REQUIRE(table[Symbol{10}] == std::set<Symbol>{Symbol{13}, Symbol{15}});

        table[Symbol{20}] = {Symbol{26}, Symbol{29}, Symbol{21}};

        REQUIRE(table[Symbol{0}] == std::set<Symbol>{Symbol{1}, Symbol{2}});

        REQUIRE(table[Symbol{10}] == std::set<Symbol>{Symbol{13}, Symbol{15}});

        REQUIRE(table[Symbol{20}] == std::set<Symbol>{Symbol{26}, Symbol{29}, Symbol{21}});

        SECTION("merge with new symbol")
        {
            table.merge({Symbol{0}, Symbol{40}});

            REQUIRE(table[Symbol{0}] == std::set<Symbol>{Symbol{1}, Symbol{2}});

            REQUIRE(table[Symbol{40}] == table[Symbol{0}]);

            REQUIRE(table[Symbol{10}] == std::set<Symbol>{Symbol{13}, Symbol{15}});

            REQUIRE(table[Symbol{20}] == std::set<Symbol>{Symbol{26}, Symbol{29}, Symbol{21}});
        }

        SECTION("merge with existing symbols")
        {
            table.merge({Symbol{0}, Symbol{20}});

            REQUIRE(table[Symbol{0}] == std::set<Symbol>{Symbol{1}, Symbol{2}, Symbol{26}, Symbol{29}, Symbol{21}});

            REQUIRE(table[Symbol{0}] == table[Symbol{20}]);

            REQUIRE(table[Symbol{10}] == std::set<Symbol>{Symbol{13}, Symbol{15}});
        }

        SECTION("iteration")
        {
            auto partitions = std::vector<std::set<Symbol>>{};
            auto values = std::vector<std::set<Symbol>>{};

            table.forEach([&partitions, &values](const auto& p, const auto& v) {
                partitions.push_back(p);
                values.push_back(v);
            });

            auto expectedPartitions = std::vector<std::set<Symbol>>{{Symbol{0}}, {Symbol{10}}, {Symbol{20}}};

            auto expectedValues = std::vector<std::set<Symbol>>{
                {Symbol{1}, Symbol{2}}, {Symbol{13}, Symbol{15}}, {Symbol{26}, Symbol{29}, Symbol{21}}};

            REQUIRE(partitions == expectedPartitions);

            REQUIRE(values == expectedValues);
        }
    }
}
