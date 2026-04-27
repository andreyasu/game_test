#include "Hunter.hpp"

#include <Core/Map.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/System/EventLog.hpp>
#include <algorithm>
#include <random>

namespace sw
{

	Hunter::Hunter(uint32_t id, Position pos, uint32_t hp, uint32_t agility, uint32_t strength, uint32_t range) :
			Unit(id, pos, static_cast<int32_t>(hp), /*mobile=*/true),
			_agility(agility),
			_strength(strength),
			_range(range)
	{}

	const char* Hunter::getTypeName() const
	{
		return "Hunter";
	}

	bool Hunter::act(Map& map, EventLog& eventLog, uint64_t tick)
	{
		const auto adjacent = map.getAdjacentUnits(getPosition(), getId());

		// 1. Swift Shot: requires no adjacent enemies and a target in [2, range]
		if (adjacent.empty())
		{
			auto ranged = map.getUnitsInRange(getPosition(), 2, _range, getId());
			if (!ranged.empty())
			{
				std::uniform_int_distribution<size_t> dist(0, ranged.size() - 1);
				Unit* target = ranged[dist(map.getRng())];

				target->takeDamage(_agility);
				const uint32_t remainingHp = (target->getHp() > 0) ? static_cast<uint32_t>(target->getHp()) : 0u;
				eventLog.log(tick, io::UnitAttacked{getId(), target->getId(), _agility, remainingHp});
				return true;
			}
		}

		// 2. Shadow Strike: melee attack on an adjacent unit
		if (!adjacent.empty())
		{
			std::uniform_int_distribution<size_t> dist(0, adjacent.size() - 1);
			Unit* target = adjacent[dist(map.getRng())];

			target->takeDamage(_strength);
			const uint32_t remainingHp = (target->getHp() > 0) ? static_cast<uint32_t>(target->getHp()) : 0u;
			eventLog.log(tick, io::UnitAttacked{getId(), target->getId(), _strength, remainingHp});
			return true;
		}

		// 3. Move toward march target
		return map.tryMoveTowardTarget(*this, eventLog, tick);
	}

}  // namespace sw
