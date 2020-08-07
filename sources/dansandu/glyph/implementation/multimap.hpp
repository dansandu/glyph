#pragma once

#include "dansandu/glyph/implementation/symbol.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::implementation::multimap
{

class Multimap
{
public:
    using Symbol = dansandu::glyph::implementation::symbol::Symbol;

    std::vector<Symbol>& operator[](Symbol key);

    void merge(std::vector<Symbol> partition);

    template<typename Lambda>
    void forEach(Lambda&& lambda) const
    {
        for (auto index = 0U; index < partitions_.size(); ++index)
        {
            lambda(partitions_[index], values_[index]);
        }
    }

private:
    std::vector<std::vector<Symbol>> partitions_;
    std::vector<std::vector<Symbol>> values_;
};

std::ostream& operator<<(std::ostream& stream, const Multimap& multimap);

}
