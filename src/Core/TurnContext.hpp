#pragma once

#include <cstdint>

namespace sw
{

	class Map;
	class Rng;
	class EventLog;

	/// @brief Everything an action needs to run during a unit's turn.
	///
	/// Bundling these into one struct keeps action signatures stable: adding a new
	/// service later (e.g. a turn counter or a team registry) does not force every
	/// existing action to change its `perform` signature.
	struct TurnContext
	{
		Map& map;
		Rng& rng;
		EventLog& log;
		uint64_t tick;
	};

}  // namespace sw
