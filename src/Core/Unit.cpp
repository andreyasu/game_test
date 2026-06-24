#include "Unit.hpp"

#include "TurnContext.hpp"

namespace sw
{

	Unit::Unit(uint32_t id, std::string typeName, Position position, UnitTraits traits) :
			_id(id),
			_typeName(std::move(typeName)),
			_position(position),
			_traits(traits),
			_hp(traits.hp)
	{}

	void Unit::takeDamage(uint32_t amount)
	{
		if (!_hp.has_value())
		{
			return;
		}

		*_hp -= static_cast<int32_t>(amount);
		if (*_hp <= 0)
		{
			_alive = false;
		}
	}

	void Unit::heal(uint32_t amount)
	{
		if (!_hp.has_value())
		{
			return;
		}

		*_hp += static_cast<int32_t>(amount);
	}

	bool Unit::act(TurnContext& context)
	{
		// Note: a unit killed earlier this same tick still takes its final turn
		// (per spec). Liveness is only checked when removing units between ticks.
		for (auto& action : _actions)
		{
			if (action->perform(*this, context))
			{
				return true;
			}
		}
		return false;
	}

}  // namespace sw
