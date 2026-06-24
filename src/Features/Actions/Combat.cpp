#include "Combat.hpp"

#include <Core/TurnContext.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::features
{

	void applyDamage(Unit& attacker, Unit& target, uint32_t amount, TurnContext& context)
	{
		target.takeDamage(amount);

		const uint32_t remainingHp = (target.getHp() > 0) ? static_cast<uint32_t>(target.getHp()) : 0u;
		context.log.log(context.tick, io::UnitAttacked{attacker.getId(), target.getId(), amount, remainingHp});
	}

}  // namespace sw::features
