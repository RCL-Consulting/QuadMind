cmake_minimum_required(VERSION 3.23)
project(QuadMind LANGUAGES CXX)

include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # /MDd friendly on MSVC

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)


# Standard & warnings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helpful MSVC defaults
if(MSVC)
  add_compile_options(/permissive- /Zc:preprocessor /W4)
endif()

# One target per .vcxproj
add_subdirectory(QMorphLib)        # QMorphLib.vcxproj is at repo root in .sln
add_subdirectory(QuadMindConsole)  # .\QuadMindConsole\QuadMindConsole.vcxproj
add_subdirectory(UnitTest)         # .\UnitTest\UnitTest.vcxproj