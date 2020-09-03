#include "dansandu/glyph/internal/item.hpp"

#include <tuple>

namespace dansandu::glyph::internal::item
{

bool operator<(const Item& left, const Item& right)
{
    return std::tie(left.ruleIndex, left.position, left.lookahead) <
           std::tie(right.ruleIndex, right.position, right.lookahead);
}

std::ostream& operator<<(std::ostream& stream, const Item& item)
{
    return stream << "Item(" << item.ruleIndex << ", " << item.position << ", " << item.lookahead << ")";
}

}
