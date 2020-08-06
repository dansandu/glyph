#pragma once

#include "dansandu/glyph/implementation/symbol.hpp"

#include <set>
#include <vector>

namespace dansandu::glyph::implementation::multimap
{

class Multimap
{
public:
    using Symbol = dansandu::glyph::implementation::symbol::Symbol;

    std::set<Symbol>& operator[](Symbol key);

    void merge(std::set<Symbol> partition);

    template<typename Lambda>
    void forEach(Lambda&& lambda) const
    {
        for (auto index = 0U; index < partitions_.size(); ++index)
        {
            lambda(partitions_[index], values_[index]);
        }
    }

private:
    std::vector<std::set<Symbol>> partitions_;
    std::vector<std::set<Symbol>> values_;
};

}
