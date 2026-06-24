#include "Simulation.hpp"

#include "TurnContext.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitSpawned.hpp>

namespace sw
{

	Simulation::Simulation(EventLog& eventLog, Rng rng) :
			_eventLog(eventLog),
			_rng(std::move(rng))
	{}

	void Simulation::createMap(uint32_t width, uint32_t height)
	{
		_map = std::make_unique<Map>(width, height);
		_eventLog.log(kSetupTick, io::MapCreated{width, height});
	}

	void Simulation::addUnit(std::shared_ptr<Unit> unit)
	{
		_eventLog.log(
			kSetupTick,
			io::UnitSpawned{unit->getId(), unit->getTypeName(), unit->getPosition().x, unit->getPosition().y});
		_map->addUnit(std::move(unit));
	}

	void Simulation::setMarch(uint32_t unitId, uint32_t targetX, uint32_t targetY)
	{
		Unit* unit = _map->getUnitById(unitId);
		if (unit == nullptr)
		{
			return;
		}

		unit->setMarchTarget({targetX, targetY});
		_eventLog.log(
			kSetupTick, io::MarchStarted{unitId, unit->getPosition().x, unit->getPosition().y, targetX, targetY});
	}

	void Simulation::removeAndAnnounceDead(uint64_t tick)
	{
		for (const auto& unit : _map->getUnits())
		{
			if (!unit->isAlive())
			{
				_eventLog.log(tick, io::UnitDied{unit->getId()});
			}
		}
		_map->removeDeadUnits();
	}

	void Simulation::run()
	{
		if (!_map)
		{
			return;
		}

		uint64_t tick = kFirstSimTick;

		// Units act in creation order. A unit that dies mid-tick still takes its
		// final turn (it is in the snapshot) and is removed before the next tick.
		while (_map->getUnits().size() >= 2)
		{
			const auto snapshot = _map->getUnits();

			bool anyActed = false;
			for (const auto& unit : snapshot)
			{
				TurnContext context{*_map, _rng, _eventLog, tick};
				if (unit->act(context))
				{
					anyActed = true;
				}
			}

			removeAndAnnounceDead(tick);

			if (!anyActed)
			{
				break;
			}

			++tick;
		}
	}

}  // namespace sw
