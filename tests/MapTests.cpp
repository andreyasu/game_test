#include "TestFramework.hpp"

#include <Core/Map.hpp>
#include <Features/Crow.hpp>
#include <Features/Swordsman.hpp>
#include <memory>

using namespace sw;

namespace
{
	std::shared_ptr<Swordsman> swordsman(uint32_t id, Position pos)
	{
		return std::make_shared<Swordsman>(id, pos, 10, 2);
	}
}

TEST(Map_bounds)
{
	Map map(10, 5);
	CHECK(map.isInBounds({0, 0}));
	CHECK(map.isInBounds({9, 4}));
	CHECK(!map.isInBounds({10, 0}));
	CHECK(!map.isInBounds({0, 5}));
}

TEST(Map_lookup_by_id)
{
	Map map(10, 10);
	map.addUnit(swordsman(7, {1, 1}));
	CHECK(map.getUnitById(7) != nullptr);
	CHECK_EQ(map.getUnitById(7)->getId(), 7u);
	CHECK(map.getUnitById(99) == nullptr);
}

TEST(Map_occupancy_ignores_flying_units)
{
	Map map(10, 10);
	map.addUnit(swordsman(1, {2, 2}));
	map.addUnit(std::make_shared<Crow>(2, Position{3, 3}, 10, 3));	// flying: does not occupy

	CHECK(map.isOccupied({2, 2}, /*excludeId=*/0));	  // ground unit blocks
	CHECK(!map.isOccupied({3, 3}, /*excludeId=*/0));  // crow does not
	CHECK(!map.isOccupied({2, 2}, /*excludeId=*/1));  // excluded
}

TEST(Map_range_queries)
{
	Map map(10, 10);
	map.addUnit(swordsman(1, {5, 5}));
	map.addUnit(swordsman(2, {6, 5}));	// dist 1
	map.addUnit(swordsman(3, {5, 8}));	// dist 3

	CHECK_EQ(map.adjacentUnits({5, 5}, 1).size(), std::size_t{1});		 // only id 2
	CHECK_EQ(map.unitsInRange({5, 5}, 2, 5, 1).size(), std::size_t{1});	 // only id 3
	CHECK_EQ(map.unitsInRange({5, 5}, 1, 5, 1).size(), std::size_t{2});	 // ids 2 and 3
}

TEST(Map_next_step_prefers_diagonal_then_falls_back)
{
	Map map(10, 10);
	auto mover = swordsman(1, {0, 0});
	map.addUnit(mover);

	auto step = map.nextStepToward({0, 0}, {5, 5}, 1);
	CHECK(step.has_value());
	CHECK(*step == (Position{1, 1}));  // diagonal preferred

	// Block the diagonal; expect an axis-aligned fallback.
	map.addUnit(swordsman(2, {1, 1}));
	auto blocked = map.nextStepToward({0, 0}, {5, 5}, 1);
	CHECK(blocked.has_value());
	CHECK(*blocked == (Position{1, 0}));

	// Block every forward candidate; expect no step.
	map.addUnit(swordsman(3, {1, 0}));
	map.addUnit(swordsman(4, {0, 1}));
	CHECK(!map.nextStepToward({0, 0}, {5, 5}, 1).has_value());
}
