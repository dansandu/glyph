#include "dansandu/glyph/implementation/multimap.hpp"

#include <algorithm>

using dansandu::glyph::implementation::symbol::Symbol;

namespace dansandu::glyph::implementation::multimap
{

std::set<Symbol>& Multimap::operator[](Symbol key)
{
    for (auto index = 0U; index < partitions_.size(); ++index)
    {
        if (partitions_[index].count(key))
        {
            return values_[index];
        }
    }
    partitions_.push_back({key});
    values_.push_back({});
    return values_.back();
}

void Multimap::merge(std::set<Symbol> partition)
{
    auto index = 0U;
    auto value = std::set<Symbol>{};
    while (index < partitions_.size())
    {
        if (std::any_of(partitions_[index].cbegin(), partitions_[index].cend(),
                        [&partition](const auto& element) { return partition.count(element); }))
        {
            partition.merge(partitions_[index]);
            value.merge(values_[index]);
            partitions_.erase(partitions_.begin() + index);
            values_.erase(values_.begin() + index);
        }
        else
        {
            ++index;
        }
    }
    partitions_.push_back(std::move(partition));
    values_.push_back(std::move(value));
}

}
