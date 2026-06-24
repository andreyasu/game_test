#include "TestFramework.hpp"

#include <Core/Map.hpp>
#include <Core/Rng.hpp>
#include <Core/TurnContext.hpp>
#include <Features/Actions/MeleeAttack.hpp>
#include <Features/Actions/RangedAttack.hpp>
#include <Features/Crow.hpp>
#include <Features/Swordsman.hpp>
#include <IO/System/EventLog.hpp>
#include <memory>
#include <sstream>

using namespace sw;

namespace
{
	std::shared_ptr<Swordsman> swordsman(uint32_t id, Position pos, uint32_t hp = 10)
	{
		return std::make_shared<Swordsman>(id, pos, hp, 2);
	}
}

TEST(Melee_hits_adjacent_and_emits_event)
{
	Map map(10, 10);
	auto attacker = swordsman(1, {1, 1});
	auto victim = swordsman(2, {1, 2}, 10);
	map.addUnit(attacker);
	map.addUnit(victim);

	std::ostringstream out;
	EventLog log(out);
	Rng rng(1);
	TurnContext ctx{map, rng, log, 2};

	features::MeleeAttack melee(3);
	CHECK(melee.perform(*attacker, ctx));
	CHECK_EQ(victim->getHp(), 7);
	CHECK_EQ(test::count(out.str(), "UNIT_ATTACKED"), std::size_t{1});
}

TEST(Melee_cannot_target_a_flying_unit)
{
	Map map(10, 10);
	auto attacker = swordsman(1, {1, 1});
	auto crow = std::make_shared<Crow>(2, Position{1, 2}, 10, 3);  // melee-immune
	map.addUnit(attacker);
	map.addUnit(crow);

	std::ostringstream out;
	EventLog log(out);
	Rng rng(1);
	TurnContext ctx{map, rng, log, 2};

	features::MeleeAttack melee(3);
	CHECK(!melee.perform(*attacker, ctx));	// no valid melee target
	CHECK_EQ(test::count(out.str(), "UNIT_ATTACKED"), std::size_t{0});
}

TEST(Ranged_respects_distance_band)
{
	Map map(10, 10);
	auto tower = swordsman(1, {0, 0});	   // shooter position only; action defines the band
	auto near = swordsman(2, {1, 0}, 10);  // dist 1 — out of [2,5]
	map.addUnit(tower);
	map.addUnit(near);

	std::ostringstream out;
	EventLog log(out);
	Rng rng(1);
	TurnContext ctx{map, rng, log, 2};

	features::RangedAttack shot(4, 2, 5, /*requireClearMelee=*/false);
	CHECK(!shot.perform(*tower, ctx));	// nothing in band

	map.addUnit(swordsman(3, {3, 0}, 10));	// dist 3 — inside [2,5]
	CHECK(shot.perform(*tower, ctx));
	CHECK_EQ(map.getUnitById(3)->getHp(), 6);
}

TEST(Ranged_reduction_lets_a_crow_be_hit_closer)
{
	Map map(10, 10);
	auto tower = swordsman(1, {0, 0});
	auto crow = std::make_shared<Crow>(2, Position{1, 0}, 20, 3);  // dist 1, reduction 1 -> band [1,4]
	map.addUnit(tower);
	map.addUnit(crow);

	std::ostringstream out;
	EventLog log(out);
	Rng rng(1);
	TurnContext ctx{map, rng, log, 2};

	features::RangedAttack shot(4, 2, 5, /*requireClearMelee=*/false);
	CHECK(shot.perform(*tower, ctx));  // crow reachable at dist 1
	CHECK_EQ(crow->getHp(), 16);
}

TEST(Ranged_clear_melee_precondition)
{
	Map map(10, 10);
	auto hunter = swordsman(1, {0, 0});
	auto target = swordsman(2, {3, 0}, 10);		// in band
	auto neighbour = swordsman(3, {1, 0}, 10);	// adjacent ground unit blocks the shot
	map.addUnit(hunter);
	map.addUnit(target);
	map.addUnit(neighbour);

	std::ostringstream out;
	EventLog log(out);
	Rng rng(1);
	TurnContext ctx{map, rng, log, 2};

	features::RangedAttack shot(4, 2, 5, /*requireClearMelee=*/true);
	CHECK(!shot.perform(*hunter, ctx));	 // blocked by adjacent ground unit
}
