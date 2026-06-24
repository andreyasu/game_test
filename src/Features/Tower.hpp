#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// @brief Stationary turret (extension plan).
	///   - Aimed Shot — shoot a random unit in [2, 5] for Power. No melee precondition.
	///   - Never moves.
	class Tower final : public Unit
	{
	public:
		Tower(uint32_t id, Position position, uint32_t hp, uint32_t power);
	};

}  // namespace sw
