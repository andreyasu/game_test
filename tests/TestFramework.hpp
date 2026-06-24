#pragma once

// A minimal, dependency-free test harness (standard library only), in keeping
// with the project's "no third-party dependencies" constraint. Define tests with
// TEST(name) { ... } and assert with CHECK / CHECK_EQ.

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace test
{

	struct Case
	{
		std::string name;
		std::function<void()> fn;
	};

	inline std::vector<Case>& registry()
	{
		static std::vector<Case> cases;
		return cases;
	}

	inline int& failureCount()
	{
		static int count = 0;
		return count;
	}

	inline void reportFailure(const std::string& message, const char* file, int line)
	{
		++failureCount();
		std::cerr << "    FAIL " << file << ":" << line << "  " << message << "\n";
	}

	struct Registrar
	{
		Registrar(std::string name, std::function<void()> fn)
		{
			registry().push_back({std::move(name), std::move(fn)});
		}
	};

	inline int run()
	{
		int passed = 0;
		for (const auto& testCase : registry())
		{
			const int before = failureCount();
			try
			{
				testCase.fn();
			}
			catch (const std::exception& e)
			{
				reportFailure(std::string("uncaught exception: ") + e.what(), __FILE__, __LINE__);
			}

			if (failureCount() == before)
			{
				++passed;
				std::cout << "[  OK  ] " << testCase.name << "\n";
			}
			else
			{
				std::cout << "[ FAIL ] " << testCase.name << "\n";
			}
		}

		std::cout << "\n" << passed << "/" << registry().size() << " tests passed";
		if (failureCount() > 0)
		{
			std::cout << " (" << failureCount() << " checks failed)\n";
			return 1;
		}
		std::cout << "\n";
		return 0;
	}

	// Counts non-overlapping occurrences of `needle` in `haystack` — handy for
	// asserting on captured event-log output.
	inline std::size_t count(const std::string& haystack, const std::string& needle)
	{
		if (needle.empty())
		{
			return 0;
		}

		std::size_t total = 0;
		std::size_t pos = 0;
		while ((pos = haystack.find(needle, pos)) != std::string::npos)
		{
			++total;
			pos += needle.size();
		}
		return total;
	}

}  // namespace test

#define TEST(name)                                                                                                     \
	static void name();                                                                                                \
	static ::test::Registrar test_registrar_##name(#name, name);                                                       \
	static void name()

#define CHECK(...)                                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(__VA_ARGS__))                                                                                            \
		{                                                                                                              \
			::test::reportFailure("CHECK failed: " #__VA_ARGS__, __FILE__, __LINE__);                                  \
		}                                                                                                              \
	}                                                                                                                  \
	while (false)

#define CHECK_EQ(a, b)                                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		const auto checkLhs = (a);                                                                                     \
		const auto checkRhs = (b);                                                                                     \
		if (!(checkLhs == checkRhs))                                                                                   \
		{                                                                                                              \
			std::ostringstream checkStream;                                                                            \
			checkStream << "CHECK_EQ failed: " #a " (" << checkLhs << ") == " #b " (" << checkRhs << ")";              \
			::test::reportFailure(checkStream.str(), __FILE__, __LINE__);                                              \
		}                                                                                                              \
	}                                                                                                                  \
	while (false)
