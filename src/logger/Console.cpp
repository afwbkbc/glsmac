#include "Console.h"

#include "task/console/Console.h"

namespace logger {

void Console::Log( const std::string& text ) {
	for ( size_t i = 0 ; i < 10 ; i++ ) {
		task::console::Console::Log( text );
	}
}

}