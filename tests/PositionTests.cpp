#include "TestFramework.hpp"

#include <Core/Position.hpp>

using sw::Position;

TEST(Position_chebyshev_diagonal_is_cheap)
{
	// A pure diagonal of length 3 is distance 3, not 6 (Manhattan).
	CHECK_EQ((Position{0, 0}.distanceTo(Position{3, 3})), 3u);
}

TEST(Position_chebyshev_takes_the_longer_axis)
{
	CHECK_EQ((Position{0, 0}.distanceTo(Position{3, 1})), 3u);
	CHECK_EQ((Position{2, 7}.distanceTo(Position{2, 2})), 5u);
}

TEST(Position_equality)
{
	CHECK(Position{4, 5} == Position{4, 5});
	CHECK(Position{4, 5} != Position{5, 4});
}
