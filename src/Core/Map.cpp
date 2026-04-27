#include "Map.hpp"

#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/System/EventLog.hpp>
#include <algorithm>

namespace sw
{

	Map::Map(uint32_t width, uint32_t height) :
			_width(width),
			_height(height),
			_rng(std::random_device{}())
	{}

	bool Map::isInBounds(Position pos) const
	{
		return pos.x < _width && pos.y < _height;
	}

	void Map::addUnit(std::shared_ptr<Unit> unit)
	{
		_units.push_back(std::move(unit));
	}

	void Map::removeDeadUnits(EventLog& eventLog, uint64_t tick)
	{
		for (const auto& unit : _units)
		{
			if (!unit->isAlive())
			{
				eventLog.log(tick, io::UnitDied{unit->getId()});
			}
		}

		_units.erase(
			std::remove_if(_units.begin(), _units.end(), [](const auto& u) { return !u->isAlive(); }), _units.end());
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

	bool Map::isOccupied(Position pos, uint32_t excludeId) const
	{
		for (const auto& unit : _units)
		{
			if (unit->getId() != excludeId && unit->isAlive() && unit->getPosition() == pos)
			{
				return true;
			}
		}
		return false;
	}

	std::vector<Unit*> Map::getAdjacentUnits(Position pos, uint32_t excludeId) const
	{
		return getUnitsInRange(pos, 1, 1, excludeId);
	}

	std::vector<Unit*> Map::getUnitsInRange(Position pos, uint32_t minDist, uint32_t maxDist, uint32_t excludeId) const
	{
		std::vector<Unit*> result;
		for (const auto& unit : _units)
		{
			if (unit->getId() == excludeId || !unit->isAlive())
			{
				continue;
			}
			uint32_t dist = pos.chebyshevDistance(unit->getPosition());
			if (dist >= minDist && dist <= maxDist)
			{
				result.push_back(unit.get());
			}
		}
		return result;
	}

	Position Map::preferredStep(Position from, Position to) const
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

		return Position{
			static_cast<uint32_t>(static_cast<int32_t>(from.x) + dx),
			static_cast<uint32_t>(static_cast<int32_t>(from.y) + dy),
		};
	}

	std::optional<Position> Map::findStep(Position from, Position to, uint32_t excludeId) const
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

		// Prefer diagonal, then axis-aligned alternatives to avoid getting stuck on corners
		const Position candidates[] = {
			{static_cast<uint32_t>(static_cast<int32_t>(from.x) + dx),
			 static_cast<uint32_t>(static_cast<int32_t>(from.y) + dy)},
			{static_cast<uint32_t>(static_cast<int32_t>(from.x) + dx), from.y},
			{from.x, static_cast<uint32_t>(static_cast<int32_t>(from.y) + dy)},
		};

		for (const auto& pos : candidates)
		{
			if (isInBounds(pos) && !isOccupied(pos, excludeId))
			{
				return pos;
			}
		}
		return std::nullopt;
	}

	bool Map::tryMoveTowardTarget(Unit& unit, EventLog& eventLog, uint64_t tick)
	{
		if (!unit.hasMarchTarget() || !unit.isMobile())
		{
			return false;
		}

		const Position target = *unit.getMarchTarget();
		const Position current = unit.getPosition();

		if (current == target)
		{
			unit.clearMarchTarget();
			eventLog.log(tick, io::MarchEnded{unit.getId(), current.x, current.y});
			return false;
		}

		auto stepOpt = findStep(current, target, unit.getId());
		if (!stepOpt)
		{
			return false;
		}

		const Position newPos = *stepOpt;
		unit.moveTo(newPos);
		eventLog.log(tick, io::UnitMoved{unit.getId(), newPos.x, newPos.y});

		if (newPos == target)
		{
			unit.clearMarchTarget();
			eventLog.log(tick, io::MarchEnded{unit.getId(), newPos.x, newPos.y});
		}

		return true;
	}

}  // namespace sw
