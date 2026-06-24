#include "Tower.hpp"

#include "Actions/RangedAttack.hpp"

namespace sw
{

	Tower::Tower(uint32_t id, Position position, uint32_t hp, uint32_t power) :
			Unit(id, "Tower", position, UnitTraits{.hp = static_cast<int32_t>(hp), .movementSpeed = 0})
	{
		addAction<features::RangedAttack>(power, 2, 5, /*requireClearMelee=*/false);
		// No MoveToTarget: a Tower is stationary.
	}

}  // namespace sw
