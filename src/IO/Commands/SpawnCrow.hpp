#pragma once

#include <cstdint>
#include <iosfwd>

namespace sw::io
{
	struct SpawnCrow
	{
		constexpr static const char* Name = "SPAWN_CROW";

		uint32_t unitId{};
		uint32_t x{};
		uint32_t y{};
		uint32_t hp{};
		uint32_t agility{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("hp", hp);
			visitor.visit("agility", agility);
		}
	};
}
