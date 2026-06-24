#include "TestFramework.hpp"

#include <Features/Crow.hpp>
#include <Features/Mine.hpp>
#include <Features/Swordsman.hpp>

using namespace sw;

TEST(Unit_swordsman_traits)
{
	Swordsman s(1, {0, 0}, 10, 2);
	CHECK_EQ(s.movementSpeed(), 1u);
	CHECK(s.occupiesCell());
	CHECK(s.canBeMeleeAttacked());
	CHECK(s.canBeRangedAttacked());
	CHECK(s.hasHealth());
}

TEST(Unit_crow_is_flying_and_melee_immune)
{
	Crow c(1, {0, 0}, 10, 3);
	CHECK_EQ(c.movementSpeed(), 2u);
	CHECK(!c.occupiesCell());
	CHECK(!c.canBeMeleeAttacked());
	CHECK(c.canBeRangedAttacked());
	CHECK_EQ(c.rangedDistanceReduction(), 1u);
}

TEST(Unit_mine_has_no_health)
{
	Mine m(1, {0, 0}, 12);
	CHECK(!m.hasHealth());
	CHECK(m.isAlive());

	m.takeDamage(100);	// no-op: a mine cannot be damaged
	CHECK(m.isAlive());

	m.kill();  // only self-removal ends a mine
	CHECK(!m.isAlive());
}

TEST(Unit_damage_and_heal)
{
	Swordsman s(1, {0, 0}, 10, 2);
	s.takeDamage(4);
	CHECK_EQ(s.getHp(), 6);
	CHECK(s.isAlive());

	s.heal(3);
	CHECK_EQ(s.getHp(), 9);

	s.takeDamage(9);  // 9 - 9 == 0
	CHECK_EQ(s.getHp(), 0);
	CHECK(!s.isAlive());
}
