#pragma once

#include "Combat.hpp"

#include <Core/IAction.hpp>
#include <Core/Map.hpp>
#include <Core/TurnContext.hpp>
#include <Core/Unit.hpp>
#include <cstdint>

namespace sw::features
{

	/// @brief Arm when a unit comes within trigger range, detonate on the next turn.
	///
	/// The Mine's whole behaviour. It is stateful (the "armed" flag), which is fine
	/// because every unit owns its own action instances. On detonation it damages all
	/// units within the blast radius and removes itself from play.
	class ExplodeIfEnemyNear final : public IAction
	{
	public:
		ExplodeIfEnemyNear(uint32_t power, uint32_t triggerRadius, uint32_t blastRadius) :
				_power(power),
				_triggerRadius(triggerRadius),
				_blastRadius(blastRadius)
		{}

		bool perform(Unit& self, TurnContext& context) override
		{
			if (_armed)
			{
				for (Unit* victim : context.map.unitsInRange(self.getPosition(), 1, _blastRadius, self.getId()))
				{
					applyDamage(self, *victim, _power, context);
				}
				self.kill();
				return true;
			}

			const bool enemyNear
				= !context.map.unitsInRange(self.getPosition(), 1, _triggerRadius, self.getId()).empty();
			if (enemyNear)
			{
				_armed = true;	// detonates next turn
				return true;
			}

			return false;
		}

	private:
		uint32_t _power;
		uint32_t _triggerRadius;
		uint32_t _blastRadius;
		bool _armed = false;
	};

}  // namespace sw::features
