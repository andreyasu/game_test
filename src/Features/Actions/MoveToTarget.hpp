#pragma once

#include <Core/IAction.hpp>
#include <Core/Map.hpp>
#include <Core/TurnContext.hpp>
#include <Core/Unit.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::features
{

	/// @brief Step toward the unit's march target, up to its movement speed.
	///
	/// The "if there is nothing better to do, move" fallback for any mobile unit.
	/// Honours movementSpeed (a Crow covers two cells), emits UNIT_MOVED per cell and
	/// MARCH_ENDED once the target is reached. Movement logic lives here, not in Map,
	/// so Map stays a pure spatial index.
	class MoveToTarget final : public IAction
	{
	public:
		bool perform(Unit& self, TurnContext& context) override
		{
			if (!self.hasMarchTarget() || self.movementSpeed() == 0)
			{
				return false;
			}

			const Position target = *self.getMarchTarget();

			if (self.getPosition() == target)
			{
				finishMarch(self, context);
				return false;
			}

			bool moved = false;
			for (uint32_t step = 0; step < self.movementSpeed(); ++step)
			{
				const Position current = self.getPosition();
				if (current == target)
				{
					break;
				}

				const auto next = context.map.nextStepToward(current, target, self.getId());
				if (!next)
				{
					break;
				}

				self.moveTo(*next);
				context.log.log(context.tick, io::UnitMoved{self.getId(), next->x, next->y});
				moved = true;

				if (*next == target)
				{
					finishMarch(self, context);
					break;
				}
			}

			return moved;
		}

	private:
		static void finishMarch(Unit& self, TurnContext& context)
		{
			self.clearMarchTarget();
			const Position position = self.getPosition();
			context.log.log(context.tick, io::MarchEnded{self.getId(), position.x, position.y});
		}
	};

}  // namespace sw::features
