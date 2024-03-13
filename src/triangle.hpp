#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <optional>

std::optional<std::pair<std::vector<float>, std::vector<uint32_t>>> GenerateSierpinskyTriangle(const float positions[6], const uint32_t indices[3], uint32_t curMaxIndex, uint8_t depth);
