#pragma once

#include "Map.hpp"
#include "Rng.hpp"

#include <IO/System/EventLog.hpp>
#include <memory>

namespace sw
{

	/// @brief Top-level orchestrator of the simulation.
	///
	/// Responsibilities:
	///   - accept setup commands (createMap, addUnit, setMarch),
	///   - run the turn loop,
	///   - emit lifecycle events: MAP_CREATED, UNIT_SPAWNED, MARCH_STARTED, UNIT_DIED.
	///
	/// Behavioural events (attacks, moves, heals) are emitted by the actions that
	/// cause them — the Simulation only owns the lifecycle.
	///
	/// Tick numbering:
	///   - tick 1: command processing (setup events),
	///   - tick 2+: simulation turns.
	///
	/// The loop ends when fewer than two units remain, or when no unit was able to
	/// act during a whole tick (prevents an infinite loop of idle units).
	class Simulation
	{
	public:
		static constexpr uint64_t kSetupTick = 1;
		static constexpr uint64_t kFirstSimTick = 2;

		/// @param eventLog sink for all emitted events.
		/// @param rng      randomness source (inject a seeded Rng for reproducible runs).
		Simulation(EventLog& eventLog, Rng rng);

		void createMap(uint32_t width, uint32_t height);
		void addUnit(std::shared_ptr<Unit> unit);
		void setMarch(uint32_t unitId, uint32_t targetX, uint32_t targetY);

		/// @brief Run the simulation to completion.
		void run();

	private:
		void removeAndAnnounceDead(uint64_t tick);

		EventLog& _eventLog;
		Rng _rng;
		std::unique_ptr<Map> _map;
	};

}  // namespace sw
