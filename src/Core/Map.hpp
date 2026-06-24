#pragma once

#include "Position.hpp"
#include "Unit.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace sw
{

	/// @brief The battlefield's spatial state: who is where, and what is around a point.
	///
	/// Single responsibility — "positions of units on a bounded grid". It owns the
	/// units and answers spatial questions about them. It deliberately does NOT:
	///   - emit events (that belongs to the Simulation and to actions),
	///   - own randomness (see Rng),
	///   - run march / turn logic (see the MoveToTarget action and Simulation).
	///
	/// Queries return *raw* sets (alive, excluding the asking unit). Combat-specific
	/// filtering (melee/ranged eligibility, range modifiers) lives in the actions, so
	/// Map carries no knowledge of attacks or healing.
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

		bool isInBounds(Position position) const;

		void addUnit(std::shared_ptr<Unit> unit);

		/// @brief Erase units that are no longer alive. Does not emit events; the
		///        Simulation announces deaths before calling this.
		void removeDeadUnits();

		/// @brief Units in creation order (used for the per-tick snapshot).
		const std::vector<std::shared_ptr<Unit>>& getUnits() const
		{
			return _units;
		}

		Unit* getUnitById(uint32_t id) const;

		/// @brief Is the cell blocked by a (living, cell-occupying) unit other than excludeId?
		bool isOccupied(Position position, uint32_t excludeId) const;

		/// @brief Living units exactly one cell away (Chebyshev distance == 1), excluding excludeId.
		std::vector<Unit*> adjacentUnits(Position position, uint32_t excludeId) const;

		/// @brief Living units with distance in [minDistance, maxDistance], excluding excludeId.
		std::vector<Unit*> unitsInRange(
			Position position, uint32_t minDistance, uint32_t maxDistance, uint32_t excludeId) const;

		/// @brief One-cell step from `from` toward `to` that stays in bounds and on a
		///        free cell. Diagonal first, then axis-aligned, to avoid sticking on
		///        corners. std::nullopt if every candidate is blocked.
		std::optional<Position> nextStepToward(Position from, Position to, uint32_t excludeId) const;

	private:
		uint32_t _width;
		uint32_t _height;
		std::vector<std::shared_ptr<Unit>> _units;
	};

}  // namespace sw
