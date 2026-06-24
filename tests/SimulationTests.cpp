#include "TestFramework.hpp"

#include <Core/Rng.hpp>
#include <Core/Simulation.hpp>
#include <Features/Crow.hpp>
#include <Features/Healer.hpp>
#include <Features/Mine.hpp>
#include <Features/Swordsman.hpp>
#include <Features/Tower.hpp>
#include <IO/System/EventLog.hpp>
#include <memory>
#include <sstream>

using namespace sw;

TEST(Sim_single_unit_does_not_start_combat)
{
	std::ostringstream out;
	EventLog log(out);
	Simulation sim(log, Rng{1});
	sim.createMap(10, 10);
	sim.addUnit(std::make_shared<Swordsman>(1, Position{0, 0}, 10, 2));
	sim.run();

	// Only setup events on tick 1; no simulation ticks happen.
	CHECK_EQ(test::count(out.str(), "[2]"), std::size_t{0});
	CHECK_EQ(test::count(out.str(), "UNIT_DIED"), std::size_t{0});
}

TEST(Sim_two_adjacent_swordsmen_fight_to_one_survivor)
{
	std::ostringstream out;
	EventLog log(out);
	Simulation sim(log, Rng{1});
	sim.createMap(10, 10);
	sim.addUnit(std::make_shared<Swordsman>(1, Position{0, 0}, 10, 5));	 // clearly stronger
	sim.addUnit(std::make_shared<Swordsman>(2, Position{1, 0}, 6, 2));
	sim.run();

	CHECK(test::count(out.str(), "UNIT_ATTACKED") > 0);
	CHECK_EQ(test::count(out.str(), "UNIT_DIED"), std::size_t{1});
}

TEST(Sim_crow_is_immune_to_enemy_melee)
{
	std::ostringstream out;
	EventLog log(out);
	Simulation sim(log, Rng{1});
	sim.createMap(10, 10);
	sim.addUnit(std::make_shared<Crow>(1, Position{1, 0}, 100, 3));		// attacks each turn
	sim.addUnit(std::make_shared<Swordsman>(2, Position{0, 0}, 9, 5));	// can never hit the crow
	sim.run();

	// The crow attacks the swordsman; the swordsman never attacks the crow.
	CHECK(test::count(out.str(), "attackerUnitId=1 targetUnitId=2") > 0);
	CHECK_EQ(test::count(out.str(), "attackerUnitId=2 targetUnitId=1"), std::size_t{0});
	CHECK_EQ(test::count(out.str(), "UNIT_DIED unitId=2"), std::size_t{1});
}

TEST(Sim_mine_arms_then_detonates)
{
	std::ostringstream out;
	EventLog log(out);
	Simulation sim(log, Rng{1});
	sim.createMap(10, 10);
	sim.addUnit(std::make_shared<Mine>(1, Position{5, 0}, 10));
	sim.addUnit(std::make_shared<Swordsman>(2, Position{0, 0}, 30, 3));
	sim.setMarch(2, 5, 0);
	sim.run();

	// Mine deals blast damage (as an attacker) and then removes itself.
	CHECK(test::count(out.str(), "attackerUnitId=1 targetUnitId=2 damage=10") > 0);
	CHECK_EQ(test::count(out.str(), "UNIT_DIED unitId=1"), std::size_t{1});
}

TEST(Sim_healer_slows_death_but_does_not_prevent_it)
{
	std::ostringstream out;
	EventLog log(out);
	Simulation sim(log, Rng{1});
	sim.createMap(10, 10);
	sim.addUnit(std::make_shared<Crow>(1, Position{0, 0}, 100, 8));		 // claw 8
	sim.addUnit(std::make_shared<Swordsman>(2, Position{1, 0}, 30, 0));	 // victim
	sim.addUnit(std::make_shared<Healer>(3, Position{3, 0}, 100, 3));	 // heal 3 < 8
	sim.run();

	CHECK(test::count(out.str(), "UNIT_HEALED") > 0);
	CHECK_EQ(test::count(out.str(), "UNIT_DIED unitId=2"), std::size_t{1});
}

TEST(Sim_tower_never_moves_even_when_ordered)
{
	std::ostringstream out;
	EventLog log(out);
	Simulation sim(log, Rng{1});
	sim.createMap(10, 10);
	// A Tower far from anything, explicitly ordered to march, plus a second unit so
	// the simulation actually runs ticks.
	sim.addUnit(std::make_shared<Tower>(1, Position{0, 0}, 20, 4));
	sim.addUnit(std::make_shared<Swordsman>(2, Position{9, 9}, 20, 2));
	sim.setMarch(1, 9, 9);	// ignored: a tower is stationary
	sim.setMarch(2, 0, 0);
	sim.run();

	// The tower never emits a move; the swordsman does.
	CHECK_EQ(test::count(out.str(), "UNIT_MOVED unitId=1"), std::size_t{0});
	CHECK(test::count(out.str(), "UNIT_MOVED unitId=2") > 0);
}
