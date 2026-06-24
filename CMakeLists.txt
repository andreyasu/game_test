cmake_minimum_required(VERSION 3.16)
project(sw_battle_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SW_WERROR "Treat warnings as errors (used in CI)" OFF)

# All sources except main.cpp build into a library, so both the executable and the
# test runner link against the exact same code.
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(sw_lib STATIC ${LIB_SOURCES})
target_include_directories(sw_lib PUBLIC src)

add_executable(sw_battle_test src/main.cpp)
target_link_libraries(sw_battle_test PRIVATE sw_lib)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	set(SW_WARNINGS -Wall -Wextra -Wpedantic)
	if(SW_WERROR)
		list(APPEND SW_WARNINGS -Werror)
	endif()
	target_compile_options(sw_lib PRIVATE ${SW_WARNINGS})
	target_compile_options(sw_battle_test PRIVATE ${SW_WARNINGS})
endif()

# --- Tests (CTest) --------------------------------------------------------------
# BUILD_TESTING is defined by include(CTest); ON by default.
include(CTest)
if(BUILD_TESTING)
	file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
	add_executable(sw_tests ${TEST_SOURCES})
	target_link_libraries(sw_tests PRIVATE sw_lib)
	target_include_directories(sw_tests PRIVATE tests)
	add_test(NAME unit_tests COMMAND sw_tests)
endif()
