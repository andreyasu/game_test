#pragma once

#include "Combat.hpp"

#include <Core/IAction.hpp>
#include <Core/Map.hpp>
#include <Core/Rng.hpp>
#include <Core/TurnContext.hpp>
#include <Core/Unit.hpp>
#include <cstdint>
#include <vector>

namespace sw::features
{

	/// @brief Hit a random adjacent unit for a fixed amount of damage.
	///
	/// Shared by every melee attacker (Swordsman's Crushing Blow, Hunter's Shadow
	/// Strike, Crow's Claw). The melee damage stat is supplied at construction, so
	/// the same class serves units whose melee damage comes from Strength, Agility,
	/// etc. Skips targets that cannot be hit in melee (e.g. a flying Crow).
	class MeleeAttack final : public IAction
	{
	public:
		explicit MeleeAttack(uint32_t damage) :
				_damage(damage)
		{}

		bool perform(Unit& self, TurnContext& context) override
		{
			std::vector<Unit*> targets;
			for (Unit* candidate : context.map.adjacentUnits(self.getPosition(), self.getId()))
			{
				if (candidate->canBeMeleeAttacked())
				{
					targets.push_back(candidate);
				}
			}

			if (targets.empty())
			{
				return false;
			}

			applyDamage(self, *context.rng.pick(targets), _damage, context);
			return true;
		}

	private:
		uint32_t _damage;
	};

}  // namespace sw::features
