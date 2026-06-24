#pragma once

#include <Core/IAction.hpp>
#include <Core/Map.hpp>
#include <Core/Rng.hpp>
#include <Core/TurnContext.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/UnitHealed.hpp>
#include <IO/System/EventLog.hpp>
#include <cstdint>
#include <vector>

namespace sw::features
{

	/// @brief Restore a fixed amount of HP to a random unit within a radius.
	///
	/// Used by the Healer's Small Heal. Only units that actually have a health pool
	/// are eligible (a Mine cannot be healed). The radius band starts at 1, so a
	/// Healer does not target itself.
	class Heal final : public IAction
	{
	public:
		Heal(uint32_t amount, uint32_t radius) :
				_amount(amount),
				_radius(radius)
		{}

		bool perform(Unit& self, TurnContext& context) override
		{
			std::vector<Unit*> targets;
			for (Unit* candidate : context.map.unitsInRange(self.getPosition(), 1, _radius, self.getId()))
			{
				if (candidate->hasHealth())
				{
					targets.push_back(candidate);
				}
			}

			if (targets.empty())
			{
				return false;
			}

			Unit* target = context.rng.pick(targets);
			target->heal(_amount);

			const uint32_t targetHp = (target->getHp() > 0) ? static_cast<uint32_t>(target->getHp()) : 0u;
			context.log.log(context.tick, io::UnitHealed{self.getId(), target->getId(), _amount, targetHp});
			return true;
		}

	private:
		uint32_t _amount;
		uint32_t _radius;
	};

}  // namespace sw::features
