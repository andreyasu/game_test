#pragma once

#include <Core/Unit.hpp>

namespace sw
{

	/// @brief Trap (extension plan).
	///   - No HP; cannot be attacked; does not occupy a cell; never moves.
	///   - Arms when a unit enters radius 2, detonates next turn dealing Power to
	///     everything within radius 3, then disappears.
	class Mine final : public Unit
	{
	public:
		Mine(uint32_t id, Position position, uint32_t power);
	};

}  // namespace sw
