#include "Swordsman.hpp"

#include "Actions/MeleeAttack.hpp"
#include "Actions/MoveToTarget.hpp"

namespace sw
{

	Swordsman::Swordsman(uint32_t id, Position position, uint32_t hp, uint32_t strength) :
			Unit(id, "Swordsman", position, UnitTraits{.hp = static_cast<int32_t>(hp)})
	{
		addAction<features::MeleeAttack>(strength);
		addAction<features::MoveToTarget>();
	}

}  // namespace sw
