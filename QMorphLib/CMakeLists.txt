# QMorphLib/CMakeLists.txt
add_library(QMorphLib STATIC)

# ---- sources ----
# (Keeping headers in target_sources so they show up nicely in IDEs.)
target_sources(QMorphLib PRIVATE
  Dart.cpp
  DelaunayMeshGen.cpp
  Edge.cpp
  Element.cpp
  GeomBasics.cpp
  GlobalSmooth.cpp
  MeshLoader.cpp
  Msg.cpp
  MyLine.cpp
  MyVector.cpp
  Node.cpp
  Numbers.cpp
  pch.cpp
  QMorph.cpp
  Quad.cpp
  Ray.cpp
  TopoCleanup.cpp
  Triangle.cpp

  Dart.h
  DelaunayMeshGen.h
  Edge.h
  Element.h
  framework.h
  Constants.h
  ArrayList.h
  GeomBasics.h
  GlobalSmooth.h
  MeshLoader.h
  MyLine.h
  MyVector.h
  Node.h
  Msg.h
  Numbers.h
  pch.h
  QMorph.h
  Quad.h
  Ray.h
  TopoCleanup.h
  Triangle.h
  Types.h
)

# ---- language / std ----
target_compile_features(QMorphLib PUBLIC cxx_std_20)

# ---- include dirs ----
# (vcxproj didn’t specify extra include paths; exposing current dir + include/ if present)
target_include_directories(QMorphLib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# ---- MSVC options to match the .vcxproj ----
if(MSVC)
  # Warning level: the vcxproj used Level3
  target_compile_options(QMorphLib PRIVATE /W3 /permissive- /Zc:preprocessor)

  # Match VS default runtime: /MDd for Debug, /MD for Release
  set_property(TARGET QMorphLib PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# ---- precompiled header ----
# The project uses pch.h / pch.cpp with PCH=Use in all configs
target_precompile_headers(QMorphLib PRIVATE pch.h)

# ---- nice Solution Explorer grouping in VS ----
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
  Dart.cpp DelaunayMeshGen.cpp Edge.cpp Element.cpp GeomBasics.cpp GlobalSmooth.cpp
  MeshLoader.cpp Msg.cpp MyLine.cpp MyVector.cpp Node.cpp Numbers.cpp pch.cpp
  QMorph.cpp Quad.cpp Ray.cpp TopoCleanup.cpp Triangle.cpp
  Dart.h DelaunayMeshGen.h Edge.h Element.h framework.h Constants.h ArrayList.h
  GeomBasics.h GlobalSmooth.h MeshLoader.h MyLine.h MyVector.h Node.h Msg.h
  Numbers.h pch.h QMorph.h Quad.h Ray.h TopoCleanup.h Triangle.h Types.h
)
