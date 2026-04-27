#include "Game.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitSpawned.hpp>

namespace sw
{

	Game::Game(EventLog& eventLog) :
			_eventLog(eventLog)
	{}

	void Game::createMap(uint32_t width, uint32_t height)
	{
		_map = std::make_unique<Map>(width, height);
		_eventLog.log(1, io::MapCreated{width, height});
	}

	void Game::addUnit(std::shared_ptr<Unit> unit)
	{
		_eventLog.log(
			1, io::UnitSpawned{unit->getId(), unit->getTypeName(), unit->getPosition().x, unit->getPosition().y});
		_map->addUnit(std::move(unit));
	}

	void Game::setMarch(uint32_t unitId, uint32_t targetX, uint32_t targetY)
	{
		Unit* unit = _map->getUnitById(unitId);
		if (!unit)
		{
			return;
		}

		unit->setMarchTarget({targetX, targetY});
		_eventLog.log(1, io::MarchStarted{unitId, unit->getPosition().x, unit->getPosition().y, targetX, targetY});
	}

	void Game::run()
	{
		if (!_map)
		{
			return;
		}

		uint64_t tick = 2;

		// Per spec: units act in creation order; dead units act their final turn
		// and are removed before the next one starts.
		while (_map->getUnits().size() >= 2)
		{
			const auto snapshot = _map->getUnits();

			bool anyActed = false;
			for (const auto& unit : snapshot)
			{
				if (unit->act(*_map, _eventLog, tick))
				{
					anyActed = true;
				}
			}

			_map->removeDeadUnits(_eventLog, tick);

			if (!anyActed)
			{
				break;
			}

			++tick;
		}
	}

}  // namespace sw
