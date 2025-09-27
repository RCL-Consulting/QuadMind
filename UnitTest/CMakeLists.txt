# UnitTest/CMakeLists.txt
add_executable(UnitTest)

target_sources(UnitTest PRIVATE
  TestArrayList.cpp
  TestEdge.cpp
  TestElement.cpp
  TestMyVector.cpp
  TestNode.cpp
  TestRay.cpp
  TestTriangle.cpp
  pch.cpp
  pch.h
)

target_compile_features(UnitTest PUBLIC cxx_std_20)

# Preprocessor macros
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions(UnitTest PRIVATE X64)
endif()
target_compile_definitions(UnitTest PRIVATE
  _CONSOLE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)

# PCH (pch.h includes gtest/gtest.h)
target_precompile_headers(UnitTest PRIVATE pch.h)

# Include your lib headers
target_include_directories(UnitTest PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../QMorphLib
)

# Link your library + GoogleTest
target_link_libraries(UnitTest PRIVATE
  QMorphLib
  GTest::gtest
  GTest::gtest_main
)

# MSVC flags (use /W4 only)
if(MSVC)
  target_compile_options(UnitTest PRIVATE /W4 /permissive- /Zc:preprocessor)
  set_property(TARGET UnitTest PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# (Optional) register with CTest
include(GoogleTest)
gtest_discover_tests(UnitTest)
