#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// Ranged/melee hybrid unit.
	///
	/// Each turn:
	///   1. **Swift Shot** — if no adjacent enemies exist and there are targets
	///      at distance [2, Range]: shoots a random one for Agility damage.
	///   2. **Shadow Strike** — if Swift Shot is not possible but an adjacent
	///      enemy exists: attacks it for Strength damage.
	///   3. If nobody to attack, moves toward march target.
	class Hunter final : public Unit
	{
	public:
		Hunter(uint32_t id, Position pos, uint32_t hp, uint32_t agility, uint32_t strength, uint32_t range);

		const char* getTypeName() const override;
		bool act(Map& map, EventLog& eventLog, uint64_t tick) override;

	private:
		uint32_t _agility;
		uint32_t _strength;
		uint32_t _range;
	};

}  // namespace sw
