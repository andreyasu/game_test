#pragma once

#include <algorithm>
#include <cstdint>

namespace sw
{

	struct Position
	{
		uint32_t x{};
		uint32_t y{};

		/// @brief Chebyshev (8-directional) distance — one diagonal step counts as 1
		uint32_t chebyshevDistance(const Position& other) const
		{
			uint32_t dx = (x >= other.x) ? (x - other.x) : (other.x - x);
			uint32_t dy = (y >= other.y) ? (y - other.y) : (other.y - y);
			return std::max(dx, dy);
		}

		bool operator==(const Position& other) const
		{
			return x == other.x && y == other.y;
		}

		bool operator!=(const Position& other) const
		{
			return !(*this == other);
		}
	};

}  // namespace sw
