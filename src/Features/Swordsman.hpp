#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// @brief Melee unit.
	///   1. Crushing Blow — hit a random adjacent unit for Strength damage.
	///   2. Otherwise move toward the march target.
	class Swordsman final : public Unit
	{
	public:
		Swordsman(uint32_t id, Position position, uint32_t hp, uint32_t strength);
	};

}  // namespace sw
