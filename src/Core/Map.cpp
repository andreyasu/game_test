#include "Map.hpp"

#include <algorithm>
#include <array>

namespace sw
{

	Map::Map(uint32_t width, uint32_t height) :
			_width(width),
			_height(height)
	{}

	bool Map::isInBounds(Position position) const
	{
		return position.x < _width && position.y < _height;
	}

	void Map::addUnit(std::shared_ptr<Unit> unit)
	{
		_units.push_back(std::move(unit));
	}

	void Map::removeDeadUnits()
	{
		_units.erase(
			std::remove_if(_units.begin(), _units.end(), [](const auto& unit) { return !unit->isAlive(); }),
			_units.end());
	}

	Unit* Map::getUnitById(uint32_t id) const
	{
		for (const auto& unit : _units)
		{
			if (unit->getId() == id)
			{
				return unit.get();
			}
		}
		return nullptr;
	}

	bool Map::isOccupied(Position position, uint32_t excludeId) const
	{
		for (const auto& unit : _units)
		{
			if (unit->getId() != excludeId && unit->isAlive() && unit->occupiesCell()
				&& unit->getPosition() == position)
			{
				return true;
			}
		}
		return false;
	}

	std::vector<Unit*> Map::adjacentUnits(Position position, uint32_t excludeId) const
	{
		return unitsInRange(position, 1, 1, excludeId);
	}

	std::vector<Unit*> Map::unitsInRange(
		Position position, uint32_t minDistance, uint32_t maxDistance, uint32_t excludeId) const
	{
		std::vector<Unit*> result;
		for (const auto& unit : _units)
		{
			if (unit->getId() == excludeId || !unit->isAlive())
			{
				continue;
			}

			const uint32_t distance = position.distanceTo(unit->getPosition());
			if (distance >= minDistance && distance <= maxDistance)
			{
				result.push_back(unit.get());
			}
		}
		return result;
	}

	std::optional<Position> Map::nextStepToward(Position from, Position to, uint32_t excludeId) const
	{
		int32_t dx = 0;
		int32_t dy = 0;

		if (from.x < to.x)
		{
			dx = 1;
		}
		else if (from.x > to.x)
		{
			dx = -1;
		}

		if (from.y < to.y)
		{
			dy = 1;
		}
		else if (from.y > to.y)
		{
			dy = -1;
		}

		// Prefer the diagonal step, then the axis-aligned alternatives, so a unit
		// slides past a blocked corner instead of stalling.
		const std::array<Position, 3> candidates = {
			Position{
				static_cast<uint32_t>(static_cast<int32_t>(from.x) + dx),
				static_cast<uint32_t>(static_cast<int32_t>(from.y) + dy)},
			Position{static_cast<uint32_t>(static_cast<int32_t>(from.x) + dx), from.y},
			Position{from.x, static_cast<uint32_t>(static_cast<int32_t>(from.y) + dy)},
		};

		for (const auto& candidate : candidates)
		{
			if (candidate != from && isInBounds(candidate) && !isOccupied(candidate, excludeId))
			{
				return candidate;
			}
		}
		return std::nullopt;
	}

}  // namespace sw
