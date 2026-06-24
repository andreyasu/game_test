#pragma once

#include <cstdint>

namespace sw::io
{
	struct UnitHealed
	{
		constexpr static const char* Name = "UNIT_HEALED";

		uint32_t healerUnitId{};
		uint32_t targetUnitId{};
		uint32_t health{};
		uint32_t targetHp{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("healerUnitId", healerUnitId);
			visitor.visit("targetUnitId", targetUnitId);
			visitor.visit("health", health);
			visitor.visit("targetHp", targetHp);
		}
	};
}
