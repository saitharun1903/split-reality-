cmake_minimum_required(VERSION 3.20)
project(SRCore LANGUAGES CXX)

# SRCore is engine-independent gameplay logic. This CMake build exists so the
# rules can be compiled and tested without Unreal Engine. Inside Unreal the same
# sources are compiled by an SRCore module (see Documentation/TECHNICAL_ARCHITECTURE.md).

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(srcore STATIC
    src/Reality.cpp
    src/RealityAssignment.cpp
    src/PhaseState.cpp
    src/CoopPuzzle.cpp
    src/MeridianGate.cpp
)
target_include_directories(srcore PUBLIC include)

# Mirror Unreal's defaults: no exceptions, no RTTI. Warnings are errors.
if(MSVC)
    target_compile_options(srcore PRIVATE /W4 /WX /GR-)
else()
    target_compile_options(srcore PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion
                                          -fno-exceptions -fno-rtti)
endif()

enable_testing()
add_executable(srcore_tests
    tests/TestMain.cpp
    tests/RealityTests.cpp
    tests/PhaseTests.cpp
    tests/PuzzleTests.cpp
)
target_link_libraries(srcore_tests PRIVATE srcore)
add_test(NAME srcore_tests COMMAND srcore_tests)
