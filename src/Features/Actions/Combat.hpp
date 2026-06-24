#pragma once

#include <cstdint>

namespace sw
{
	class Unit;
	struct TurnContext;
}

namespace sw::features
{

	/// @brief Apply damage from `attacker` to `target` and emit UNIT_ATTACKED.
	///
	/// The single place that mutates HP via an attack and reports it, shared by
	/// melee, ranged and explosion actions so the "deal damage + log" tail is never
	/// duplicated.
	void applyDamage(Unit& attacker, Unit& target, uint32_t amount, TurnContext& context);

}  // namespace sw::features
