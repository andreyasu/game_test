#pragma once

#include <cstdint>
#include <random>
#include <vector>

namespace sw
{

	/// @brief Source of randomness for the simulation.
	///
	/// Extracted into its own type so that randomness is an explicit, injectable
	/// dependency rather than a hidden responsibility of Map. Actions receive it
	/// through TurnContext and never reach for a global generator.
	class Rng
	{
	public:
		/// @brief Non-deterministic generator (default for normal runs).
		Rng() :
				_engine(std::random_device{}())
		{}

		/// @brief Deterministic generator — useful for reproducing a scenario while debugging.
		explicit Rng(uint32_t seed) :
				_engine(seed)
		{}

		/// @brief Returns a uniformly chosen element of a non-empty container.
		/// @pre items must not be empty.
		template <typename T>
		T pick(const std::vector<T>& items)
		{
			std::uniform_int_distribution<std::size_t> dist(0, items.size() - 1);
			return items[dist(_engine)];
		}

	private:
		std::mt19937 _engine;
	};

}  // namespace sw
