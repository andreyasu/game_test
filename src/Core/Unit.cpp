#include "Unit.hpp"

namespace sw
{

	Unit::Unit(uint32_t id, Position pos, int32_t hp, bool mobile) :
			_id(id),
			_pos(pos),
			_hp(hp),
			_mobile(mobile)
	{}

	void Unit::takeDamage(uint32_t amount)
	{
		_hp -= static_cast<int32_t>(amount);
	}

}  // namespace sw
