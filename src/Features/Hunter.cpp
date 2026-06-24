#include "Hunter.hpp"

#include "Actions/MeleeAttack.hpp"
#include "Actions/MoveToTarget.hpp"
#include "Actions/RangedAttack.hpp"

namespace sw
{

	Hunter::Hunter(uint32_t id, Position position, uint32_t hp, uint32_t agility, uint32_t strength, uint32_t range) :
			Unit(id, "Hunter", position, UnitTraits{.hp = static_cast<int32_t>(hp)})
	{
		addAction<features::RangedAttack>(agility, 2, range, /*requireClearMelee=*/true);
		addAction<features::MeleeAttack>(strength);
		addAction<features::MoveToTarget>();
	}

}  // namespace sw
