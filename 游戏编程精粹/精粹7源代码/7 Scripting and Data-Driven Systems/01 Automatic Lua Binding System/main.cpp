#include "script.h"
#include "script_manager.h"
#include <string>

void main( int argc, char argv[] )
{
    SCRIPT_MANAGER
        manager;
    SCRIPT_REF
        script;
    std::string
        text;

    text = "player";

    script = manager.CreateScript();

    script->ExecuteFile( "../test.lua" );

    script->CallFunction( "Initialize" );
    script->CallFunction( "PrintObjectHealth", text );

	script.SetNull();

}