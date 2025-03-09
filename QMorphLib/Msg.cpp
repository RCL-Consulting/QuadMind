#include "pch.h"
#include "Msg.h"

#include <iostream>

void 
Msg::error( const std::string& err )
{
	std::cout << "Error: " << err << std::endl;
	/*Frame f = new Frame();
	MsgDialog errorDialog;
	Error error = new Error( err );
	error.printStackTrace();

	errorDialog = new MsgDialog( f, "Program error", "Program error.\nSee the log-file for details.", 40, 2 );
	errorDialog.show();

	System.exit( 1 );*/
}

void 
Msg::warning( const std::string& warn )
{
	if ( debugMode )
	{
		std::cout << "Warning: " << warn << std::endl;
	}
}

void 
Msg::debug( const std::string& msg )
{
	if ( debugMode )
	{
		std::cout << "Debug: " << msg << std::endl;
	}
}