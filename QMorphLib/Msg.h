#pragma once

#include <string>
/**
 * This class outputs messages to the user.
 */

class Msg
{
public:
	inline static bool debugMode = true;

	/** Output an error message and then exit the program. */
	static void error( const std::string& err );

	/** Output a warning message. */
	static void warning( const std::string& warn );

	/** Output a debug message. */
	static void debug( const std::string& msg );
};