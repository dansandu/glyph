#pragma once

#include "dansandu/glyph/token.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace dansandu::glyph::implementation::tokenization {

class TokenizationError : public std::runtime_error {
    using runtime_error::runtime_error;
};

std::string getTerminalsPattern(const std::vector<dansandu::glyph::token::TokenDescriptor>& tokenDescriptors);

std::vector<dansandu::glyph::token::Token>
tokenize(std::string_view string, const std::vector<dansandu::glyph::token::TokenDescriptor>& tokenDescriptors,
         const std::vector<std::string>& discard = {});
}
