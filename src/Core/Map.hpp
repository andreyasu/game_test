#pragma once

#include "Unit.hpp"

#include <memory>
#include <optional>
#include <random>
#include <vector>

namespace sw
{

	class EventLog;

	// Owns all units and provides spatial services to Features/ code:
	//   - unit bookkeeping and lookup
	//   - spatial queries (adjacent, range) using Chebyshev distance
	//   - one-step movement toward a march target
	//   - shared RNG for random target selection
	//
	// Adding new unit types in Features/ requires no changes here.
	class Map
	{
	public:
		Map(uint32_t width, uint32_t height);

		uint32_t getWidth() const
		{
			return _width;
		}

		uint32_t getHeight() const
		{
			return _height;
		}

		bool isInBounds(Position pos) const;

		void addUnit(std::shared_ptr<Unit> unit);

		// Emits UNIT_DIED for each dead unit and erases them. Call at end of tick.
		void removeDeadUnits(EventLog& eventLog, uint64_t tick);

		// Units in creation order — used for per-tick snapshot iteration.
		const std::vector<std::shared_ptr<Unit>>& getUnits() const
		{
			return _units;
		}

		Unit* getUnitById(uint32_t id) const;

		bool isOccupied(Position pos, uint32_t excludeId) const;

		// Living units at Chebyshev distance == 1, excluding excludeId.
		std::vector<Unit*> getAdjacentUnits(Position pos, uint32_t excludeId) const;

		// Living units with Chebyshev distance in [minDist, maxDist], excluding excludeId.
		std::vector<Unit*> getUnitsInRange(Position pos, uint32_t minDist, uint32_t maxDist, uint32_t excludeId) const;

		// Moves unit one step toward its march target. Emits UNIT_MOVED and
		// MARCH_ENDED when target is reached. Returns true if the unit moved.
		bool tryMoveTowardTarget(Unit& unit, EventLog& eventLog, uint64_t tick);

		std::mt19937& getRng()
		{
			return _rng;
		}

	private:
		uint32_t _width;
		uint32_t _height;
		std::vector<std::shared_ptr<Unit>> _units;
		std::mt19937 _rng;

		// Returns the best available step toward `to`: diagonal first, then axis-aligned.
		std::optional<Position> findStep(Position from, Position to, uint32_t excludeId) const;
	};

}  // namespace sw
