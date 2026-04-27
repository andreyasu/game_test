#pragma once

#include "Map.hpp"

#include <IO/System/EventLog.hpp>
#include <memory>

namespace sw
{

	/// Top-level simulation orchestrator.
	///
	/// Responsibilities:
	///   - Accept setup commands (createMap, addUnit, setMarch)
	///   - Run the turn-based simulation loop in run()
	///
	/// The loop terminates when fewer than two units remain OR when no unit
	/// was able to act during a full tick (prevents infinite loops with idle units).
	///
	/// Tick numbering:
	///   - Tick 1: command processing (MAP_CREATED, UNIT_SPAWNED, MARCH_STARTED)
	///   - Tick 2+: simulation ticks
	class Game
	{
	public:
		static constexpr uint64_t kSetupTick = 1;
		static constexpr uint64_t kFirstSimTick = 2;

		explicit Game(EventLog& eventLog);

		void createMap(uint32_t width, uint32_t height);
		void addUnit(std::shared_ptr<Unit> unit);
		void setMarch(uint32_t unitId, uint32_t targetX, uint32_t targetY);

		/// @brief Start and run the simulation to completion.
		void run();

	private:
		EventLog& _eventLog;
		std::unique_ptr<Map> _map;
	};

}  // namespace sw
