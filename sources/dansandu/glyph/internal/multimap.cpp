#include "dansandu/glyph/internal/multimap.hpp"
#include "dansandu/ballotin/string.hpp"

#include <algorithm>

using dansandu::ballotin::string::join;
using dansandu::glyph::symbol::Symbol;

namespace dansandu::glyph::internal::multimap
{

template<typename T>
auto find(const std::vector<T>& container, const T& value)
{
    return std::find(container.cbegin(), container.cend(), value);
}

std::vector<Symbol>& Multimap::operator[](const Symbol key)
{
    for (auto index = 0U; index < partitions_.size(); ++index)
    {
        if (find(partitions_[index], key) != partitions_[index].cend())
        {
            return values_[index];
        }
    }
    partitions_.push_back({key});
    values_.push_back({});
    return values_.back();
}

void Multimap::merge(std::vector<Symbol> partition)
{
    auto index = 0U;
    auto value = std::vector<Symbol>{};
    while (index < partitions_.size())
    {
        if (std::any_of(partitions_[index].cbegin(), partitions_[index].cend(),
                        [&partition](auto symbol) { return find(partition, symbol) != partition.cend(); }))
        {
            for (const auto& symbol : partitions_[index])
            {
                if (find(partition, symbol) == partition.cend())
                {
                    partition.push_back(symbol);
                }
            }
            for (const auto& symbol : values_[index])
            {
                if (find(value, symbol) == value.cend())
                {
                    value.push_back(symbol);
                }
            }
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

std::ostream& operator<<(std::ostream& stream, const Multimap& multimap)
{
    auto parts = std::vector<std::string>{};
    multimap.forEach([&parts](const auto& partition, const auto& values) {
        parts.push_back("{" + join(partition, ", ") + "}: {" + join(values, ", ") + "}");
    });
    return stream << "{" << join(parts, ", ") << "}";
}

}
