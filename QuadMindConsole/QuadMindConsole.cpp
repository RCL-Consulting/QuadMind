// QuadMindConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GeomBasics.h"
#include "QMorph.h"

#include <iostream>
#include <filesystem>

int main( int argc, char* argv[] )
{
	if ( argc < 2 )
	{
		std::cout << "Usage: QuadMindConsole <input file>\n";
		return 1;
	}

	std::string inputFilename = argv[1];
	std::filesystem::path inputPath( inputFilename );

	GeomBasics::clearLists();

	GeomBasics::setParams( inputPath.filename().string(), inputPath.parent_path().string(), false, false);
	GeomBasics::loadMesh();
	GeomBasics::findExtremeNodes();

	auto Morph = std::make_shared<QMorph>();
	Morph->init();
	Morph->run();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
