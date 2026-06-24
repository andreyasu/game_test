#pragma once

#include "details/PrintFieldVisitor.hpp"

#include <cstdint>
#include <iostream>
#include <ostream>

namespace sw
{
	/// @brief Sink for all simulation events.
	///
	/// Writes to an injectable std::ostream (defaults to std::cout) so the output
	/// can be captured verbatim in tests. The on-the-wire format is unchanged.
	class EventLog
	{
	public:
		explicit EventLog(std::ostream& out = std::cout) :
				_out(out)
		{}

		template <class TEvent>
		void log(uint64_t tick, TEvent&& event)
		{
			_out << "[" << tick << "] " << TEvent::Name << " ";
			PrintFieldVisitor visitor(_out);
			event.visit(visitor);
			_out << std::endl;
		}

	private:
		std::ostream& _out;
	};
}
