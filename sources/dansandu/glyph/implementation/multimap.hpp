#pragma once

#include "dansandu/glyph/implementation/symbol.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::implementation::multimap
{

class Multimap
{
public:
    std::vector<dansandu::glyph::implementation::symbol::Symbol>&
    operator[](dansandu::glyph::implementation::symbol::Symbol key);

    void merge(std::vector<dansandu::glyph::implementation::symbol::Symbol> partition);

    template<typename Lambda>
    void forEach(Lambda&& lambda) const
    {
        for (auto index = 0U; index < partitions_.size(); ++index)
        {
            lambda(partitions_[index], values_[index]);
        }
    }

private:
    std::vector<std::vector<dansandu::glyph::implementation::symbol::Symbol>> partitions_;
    std::vector<std::vector<dansandu::glyph::implementation::symbol::Symbol>> values_;
};

std::ostream& operator<<(std::ostream& stream, const Multimap& multimap);

}
