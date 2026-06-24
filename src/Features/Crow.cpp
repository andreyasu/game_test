#include "Crow.hpp"

#include "Actions/MeleeAttack.hpp"
#include "Actions/MoveToTarget.hpp"

namespace sw
{

	Crow::Crow(uint32_t id, Position position, uint32_t hp, uint32_t agility) :
			Unit(
				id,
				"Crow",
				position,
				UnitTraits{
					.hp = static_cast<int32_t>(hp),
					.movementSpeed = 2,
					.occupiesCell = false,
					.meleeTargetable = false,
					.rangedDistanceReduction = 1})
	{
		addAction<features::MeleeAttack>(agility);
		addAction<features::MoveToTarget>();
	}

}  // namespace sw
