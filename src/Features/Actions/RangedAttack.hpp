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

	/// @brief Shoot a random unit within a distance band for a fixed amount of damage.
	///
	/// Shared by the Hunter's Swift Shot and the Tower's Aimed Shot. Two parameters
	/// capture the differences between shooters:
	///   - [minDistance, maxDistance]: the nominal range band.
	///   - requireClearMelee: if true, the shooter may fire only when no
	///     cell-occupying unit stands adjacent (the Hunter); the Tower sets false.
	///
	/// Per-target range modifiers (a flying Crow shrinks the band by one on both
	/// ends) are honoured here, so the rule lives with the attack rather than in Core.
	class RangedAttack final : public IAction
	{
	public:
		RangedAttack(uint32_t damage, uint32_t minDistance, uint32_t maxDistance, bool requireClearMelee) :
				_damage(damage),
				_minDistance(minDistance),
				_maxDistance(maxDistance),
				_requireClearMelee(requireClearMelee)
		{}

		bool perform(Unit& self, TurnContext& context) override
		{
			if (_requireClearMelee && hasAdjacentBlocker(self, context))
			{
				return false;
			}

			std::vector<Unit*> targets;
			// Reductions only shrink the band, so [1, maxDistance] is a safe superset.
			for (Unit* candidate : context.map.unitsInRange(self.getPosition(), 1, _maxDistance, self.getId()))
			{
				if (!candidate->canBeRangedAttacked())
				{
					continue;
				}

				const uint32_t reduction = candidate->rangedDistanceReduction();
				const uint32_t effectiveMin = (_minDistance > reduction) ? _minDistance - reduction : 0;
				const uint32_t effectiveMax = (_maxDistance > reduction) ? _maxDistance - reduction : 0;
				const uint32_t distance = self.getPosition().distanceTo(candidate->getPosition());

				if (distance >= effectiveMin && distance <= effectiveMax)
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
		static bool hasAdjacentBlocker(const Unit& self, TurnContext& context)
		{
			for (Unit* neighbour : context.map.adjacentUnits(self.getPosition(), self.getId()))
			{
				// Only ground (cell-occupying) units block a shot; a flying unit
				// overhead does not count as "an adjacent unit" for this rule.
				if (neighbour->occupiesCell())
				{
					return true;
				}
			}
			return false;
		}

		uint32_t _damage;
		uint32_t _minDistance;
		uint32_t _maxDistance;
		bool _requireClearMelee;
	};

}  // namespace sw::features
