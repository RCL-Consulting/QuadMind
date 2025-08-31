#include "pch.h"
#include "Msg.h"

#include <iostream>
#include <chrono>
#include <ctime>

void Msg::initLog( const std::string& filename )
{
    std::lock_guard<std::mutex> lk( logMutex );
    logFile.open( filename, std::ios::out | std::ios::trunc );
    if ( !logFile )
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Msg::shutdownLog()
{
    std::lock_guard<std::mutex> lk( logMutex );
    if ( logFile.is_open() )
        logFile.close();
}

void Msg::write( const std::string& level, const std::string& msg )
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto timeT = system_clock::to_time_t( now );
    char buf[32];
    strftime( buf, sizeof( buf ), "%Y-%m-%d %H:%M:%S", std::localtime( &timeT ) );

    std::lock_guard<std::mutex> lk( logMutex );

    std::string line = "[" + std::string( buf ) + "] " + level + ": " + msg + "\n";

    // Console
    if ( debugMode )
        std::cout << line;

    // File
    if ( logFile.is_open() )
    {
        logFile << line;
        logFile.flush();
    }
}

void Msg::error( const std::string& err )
{
    write( "Error", err );
    // exit after flushing
    shutdownLog();
    std::exit( 1 );
}

void Msg::warning( const std::string& warn )
{
    write( "Warning", warn );
}

void Msg::debug( const std::string& msg )
{
    write( "Debug", msg );
}
