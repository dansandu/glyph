#include "dansandu/glyph/internal/item.hpp"

#include <ostream>

namespace dansandu::glyph::internal::item
{

std::ostream& operator<<(std::ostream& stream, const Item& item)
{
    return stream << "Item(" << item.ruleIndex << ", " << item.position << ", " << item.lookahead << ")";
}

}
