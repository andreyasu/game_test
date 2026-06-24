#include "Healer.hpp"

#include "Actions/Heal.hpp"
#include "Actions/MoveToTarget.hpp"

namespace sw
{

	Healer::Healer(uint32_t id, Position position, uint32_t hp, uint32_t spirit) :
			Unit(id, "Healer", position, UnitTraits{.hp = static_cast<int32_t>(hp)})
	{
		addAction<features::Heal>(spirit, 2);
		addAction<features::MoveToTarget>();
	}

}  // namespace sw
