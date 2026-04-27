#include "Swordsman.hpp"

#include <Core/Map.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/System/EventLog.hpp>
#include <algorithm>
#include <random>

namespace sw
{

	Swordsman::Swordsman(uint32_t id, Position pos, uint32_t hp, uint32_t strength) :
			Unit(id, pos, static_cast<int32_t>(hp), /*mobile=*/true),
			_strength(strength)
	{}

	const char* Swordsman::getTypeName() const
	{
		return "Swordsman";
	}

	bool Swordsman::act(Map& map, EventLog& eventLog, uint64_t tick)
	{
		// 1. Crushing Blow: attack a random adjacent unit
		auto adjacent = map.getAdjacentUnits(getPosition(), getId());
		if (!adjacent.empty())
		{
			std::uniform_int_distribution<size_t> dist(0, adjacent.size() - 1);
			Unit* target = adjacent[dist(map.getRng())];

			target->takeDamage(_strength);
			const uint32_t remainingHp = (target->getHp() > 0) ? static_cast<uint32_t>(target->getHp()) : 0u;
			eventLog.log(tick, io::UnitAttacked{getId(), target->getId(), _strength, remainingHp});
			return true;
		}

		// 2. Move toward march target
		return map.tryMoveTowardTarget(*this, eventLog, tick);
	}

}  // namespace sw
