#include "Mine.hpp"

#include "Actions/ExplodeIfEnemyNear.hpp"

namespace sw
{

	Mine::Mine(uint32_t id, Position position, uint32_t power) :
			Unit(
				id,
				"Mine",
				position,
				UnitTraits{
					.hp = std::nullopt,	 // a Mine has no health pool at all
					.movementSpeed = 0,
					.occupiesCell = false,
					.meleeTargetable = false,
					.rangedTargetable = false})
	{
		addAction<features::ExplodeIfEnemyNear>(power, /*triggerRadius=*/2, /*blastRadius=*/3);
	}

}  // namespace sw
