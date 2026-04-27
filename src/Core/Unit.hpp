#pragma once

#include "Position.hpp"

#include <cstdint>
#include <optional>

namespace sw
{

	class Map;
	class EventLog;

	// Base class for all units. Concrete types live in Features/ and implement
	// getTypeName() and act(). Core never depends on a specific unit type.
	class Unit
	{
	public:
		Unit(uint32_t id, Position pos, int32_t hp, bool mobile);
		virtual ~Unit() = default;

		uint32_t getId() const
		{
			return _id;
		}

		virtual const char* getTypeName() const = 0;

		Position getPosition() const
		{
			return _pos;
		}

		void moveTo(Position pos)
		{
			_pos = pos;
		}

		int32_t getHp() const
		{
			return _hp;
		}

		bool isAlive() const
		{
			return _hp > 0;
		}

		void takeDamage(uint32_t amount);

		bool isMobile() const
		{
			return _mobile;
		}

		bool hasMarchTarget() const
		{
			return _marchTarget.has_value();
		}

		std::optional<Position> getMarchTarget() const
		{
			return _marchTarget;
		}

		void setMarchTarget(Position target)
		{
			_marchTarget = target;
		}

		void clearMarchTarget()
		{
			_marchTarget.reset();
		}

		// Returns true if the unit did something (attack or move), false if idle.
		virtual bool act(Map& map, EventLog& eventLog, uint64_t tick) = 0;

	private:
		uint32_t _id;
		Position _pos;
		int32_t _hp;
		bool _mobile;
		std::optional<Position> _marchTarget;
	};

}  // namespace sw
