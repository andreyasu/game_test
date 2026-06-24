#pragma once

#include <algorithm>
#include <cstdint>

namespace sw
{

	/// @brief A cell on the battlefield grid.
	struct Position
	{
		uint32_t x{};
		uint32_t y{};

		/// @brief Chebyshev (8-directional) distance: one diagonal step counts as 1.
		///
		/// Natural for an 8-neighbour grid where a diagonal move costs the same as
		/// an orthogonal one. Every spatial query in the project uses this metric so
		/// that "distance" means the same thing for movement and for attack ranges.
		uint32_t distanceTo(const Position& other) const
		{
			const uint32_t dx = (x >= other.x) ? (x - other.x) : (other.x - x);
			const uint32_t dy = (y >= other.y) ? (y - other.y) : (other.y - y);
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
