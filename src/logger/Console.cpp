#include "Console.h"

#include "task/console/Console.h"

namespace logger {

void Console::Log( const std::string& text ) {
	task::console::Console::Log( text );
}

}