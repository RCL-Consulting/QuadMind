cmake_minimum_required(VERSION 3.10)
project(QMorphLib)

set(CMAKE_CXX_STANDARD 20)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# List your source files
set(SOURCES
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
)

set(HEADERS
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

# Create a library target
add_library(QMorphLib STATIC ${SOURCES} ${HEADERS})