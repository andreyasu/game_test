#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// @brief Support unit (extension plan).
	///   1. Small Heal — restore Spirit HP to a random unit within radius 2.
	///   2. Otherwise move toward the march target.
	class Healer final : public Unit
	{
	public:
		Healer(uint32_t id, Position position, uint32_t hp, uint32_t spirit);
	};

}  // namespace sw
