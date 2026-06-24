#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// @brief Ranged / melee hybrid.
	///   1. Swift Shot — if no ground unit is adjacent, shoot a unit in [2, Range] for Agility.
	///   2. Shadow Strike — otherwise hit a random adjacent unit for Strength.
	///   3. Otherwise move toward the march target.
	class Hunter final : public Unit
	{
	public:
		Hunter(uint32_t id, Position position, uint32_t hp, uint32_t agility, uint32_t strength, uint32_t range);
	};

}  // namespace sw
