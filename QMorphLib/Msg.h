#pragma once

#include <string>
#include <fstream>
#include <mutex>

/**
 * This class outputs messages to the user.
 * Now supports dumping to a log file for post-mortem debugging.
 */
class Msg
{
public:
    inline static bool debugMode = true;

    /// Call once at program start to open a logfile
    static void initLog( const std::string& filename = "Qmorph.log" );

    /// Clean up / flush the logfile
    static void shutdownLog();

    /** Output an error message and then exit the program. */
    static void error( const std::string& err );

    /** Output a warning message. */
    static void warning( const std::string& warn );

    /** Output a debug message. */
    static void debug( const std::string& msg );

private:
    inline static std::ofstream logFile;
    inline static std::mutex logMutex; // guard multi-threaded output
    static void write( const std::string& level, const std::string& msg );
};
