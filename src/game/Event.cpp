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
		case ET_TURN_COMPLETE_STATUS: {
			data.turn_complete_status.is_turn_complete = other.data.turn_complete_status.is_turn_complete;
			break;
		}
		case ET_SLOT_DEFINE: {
			NEW( data.slot_define.slotdefs, slot_defines_t, *other.data.slot_define.slotdefs );
			break;
		}
		case ET_UNIT_DEFINE: {
			NEW( data.unit_define.serialized_unitdef, std::string, *other.data.unit_define.serialized_unitdef );
			break;
		}
		case ET_UNIT_SPAWN: {
			data.unit_spawn.unit_id = other.data.unit_spawn.unit_id;
			NEW( data.unit_spawn.unitdef_name, std::string, *other.data.unit_spawn.unitdef_name );
			data.unit_spawn.slot_index = other.data.unit_spawn.slot_index;
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
		case ET_SLOT_DEFINE: {
			DELETE( data.slot_define.slotdefs );
			break;
		}
		case ET_UNIT_DEFINE: {
			DELETE( data.unit_define.serialized_unitdef );
			break;
		}
		case ET_UNIT_SPAWN: {
			DELETE( data.unit_spawn.unitdef_name );
			break;
		}
		default: {
			//
		}
	}
}

}
