#pragma once

#include "dansandu/glyph/symbol.hpp"

#include <ostream>
#include <vector>

namespace dansandu::glyph::internal::multimap
{

class Multimap
{
public:
    std::vector<dansandu::glyph::symbol::Symbol>& operator[](const dansandu::glyph::symbol::Symbol key);

    void merge(std::vector<dansandu::glyph::symbol::Symbol> partition);

    template<typename Lambda>
    void forEach(Lambda&& lambda) const
    {
        for (auto index = 0U; index < partitions_.size(); ++index)
        {
            lambda(partitions_[index], values_[index]);
        }
    }

private:
    std::vector<std::vector<dansandu::glyph::symbol::Symbol>> partitions_;
    std::vector<std::vector<dansandu::glyph::symbol::Symbol>> values_;
};

std::ostream& operator<<(std::ostream& stream, const Multimap& multimap);

}
