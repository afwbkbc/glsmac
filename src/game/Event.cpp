#include "Event.h"

#include <cstring>

#include "base/Base.h"

namespace game {

Event::Event( const Event::event_type_t type )
	: type( type ) {
	memset( &data, 0, sizeof( data ) );
}

Event::Event( const Event& other )
	: type( other.type ) {
	switch ( type ) {
		case ET_QUIT: {
			NEW( data.quit.reason, std::string, *other.data.quit.reason );
			break;
		}
		case ET_ERROR: {
			NEW( data.error.what, std::string, *other.data.error.what );
			NEW( data.error.stacktrace, std::string, *other.data.error.stacktrace );
			break;
		}
		case ET_GLOBAL_MESSAGE: {
			NEW( data.global_message.message, std::string, *other.data.global_message.message );
			break;
		}
		case ET_UNIT_SPAWN: {
			NEW( data.unit_spawn.serialized_unit, std::string, *other.data.unit_spawn.serialized_unit );
			data.unit_spawn.coords = other.data.unit_spawn.coords;
			break;
		}
		case ET_UNIT_DESPAWN: {
			data.unit_despawn.unit_id = other.data.unit_despawn.unit_id;
			break;
		}
		default: {
			//
		}
	}
}

Event::~Event() {
	switch ( type ) {
		case ET_QUIT: {
			DELETE( data.quit.reason );
			break;
		}
		case ET_ERROR: {
			DELETE( data.error.what );
			DELETE( data.error.stacktrace );
			break;
		}
		case ET_GLOBAL_MESSAGE: {
			DELETE( data.global_message.message );
			break;
		}
		case ET_UNIT_SPAWN: {
			DELETE( data.unit_spawn.serialized_unit );
			break;
		}
		default: {
			//
		}
	}
}

}
