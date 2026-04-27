#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// Melee unit.
	///
	/// Each turn:
	///   1. **Crushing Blow** — attacks a random adjacent unit for Strength damage.
	///   2. If no adjacent targets, moves toward march target.
	class Swordsman final : public Unit
	{
	public:
		Swordsman(uint32_t id, Position pos, uint32_t hp, uint32_t strength);

		const char* getTypeName() const override;
		bool act(Map& map, EventLog& eventLog, uint64_t tick) override;

	private:
		uint32_t _strength;
	};

}  // namespace sw
