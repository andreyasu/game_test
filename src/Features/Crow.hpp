#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// @brief Flying unit (extension plan).
	///   - Does not occupy a cell; moves up to 2 cells per turn.
	///   - Cannot be hit in melee; ranged attacks against it lose 1 from both range bounds.
	///   1. Claw Strike — hit a random adjacent unit for Agility.
	///   2. Otherwise move toward the march target.
	class Crow final : public Unit
	{
	public:
		Crow(uint32_t id, Position position, uint32_t hp, uint32_t agility);
	};

}  // namespace sw
