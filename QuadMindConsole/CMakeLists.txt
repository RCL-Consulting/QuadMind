# QuadMindConsole/CMakeLists.txt
add_executable(QuadMindConsole)

# ---- sources ----
target_sources(QuadMindConsole PRIVATE
  QuadMindConsole.cpp
)

# ---- language / std ----
target_compile_features(QuadMindConsole PUBLIC cxx_std_20)

# ---- include paths (from .vcxproj) ----
# The x64 configs had "..\QMorphLib" as an additional include dir.
# Using a relative path that works from this folder:
target_include_directories(QuadMindConsole PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../QMorphLib
)

# ---- preprocessor macros (from .vcxproj) ----
target_compile_definitions(QuadMindConsole PRIVATE
  _CONSOLE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)

# ---- warnings / MSVC options ----
if(MSVC)
  target_compile_options(QuadMindConsole PRIVATE /W4 /permissive- /Zc:preprocessor)
  # VS default runtime: /MDd (Debug), /MD (Release)
  set_property(TARGET QuadMindConsole PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# ---- link deps ----
# The .vcxproj links QMorphLib.lib and adds the solution’s {Platform}\{Configuration}
# lib dir. In CMake, just link the target directly; no manual lib dirs needed.
target_link_libraries(QuadMindConsole PRIVATE QMorphLib)

# ---- (optional) nice Solution Explorer grouping ----
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES QuadMindConsole.cpp)
