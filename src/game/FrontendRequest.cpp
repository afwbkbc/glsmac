#include "FrontendRequest.h"

#include <cstring>

#include "base/Base.h"

namespace game {

FrontendRequest::FrontendRequest( const request_type_t type )
	: type( type ) {
	memset( &data, 0, sizeof( data ) );
}

FrontendRequest::FrontendRequest( const FrontendRequest& other )
	: type( other.type ) {
	switch ( type ) {
		case FR_QUIT: {
			NEW( data.quit.reason, std::string, *other.data.quit.reason );
			break;
		}
		case FR_ERROR: {
			NEW( data.error.what, std::string, *other.data.error.what );
			NEW( data.error.stacktrace, std::string, *other.data.error.stacktrace );
			break;
		}
		case FR_GLOBAL_MESSAGE: {
			NEW( data.global_message.message, std::string, *other.data.global_message.message );
			break;
		}
		case FR_TURN_ACTIVE_STATUS: {
			data.turn_active_status = other.data.turn_active_status;
			break;
		}
		case FR_TURN_COMPLETE_STATUS: {
			data.turn_complete_status = other.data.turn_complete_status;
			break;
		}
		case FR_SLOT_DEFINE: {
			NEW( data.slot_define.slotdefs, slot_defines_t, *other.data.slot_define.slotdefs );
			break;
		}
		case FR_UNIT_DEFINE: {
			NEW( data.unit_define.serialized_unitdef, std::string, *other.data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_SPAWN: {
			data.unit_spawn = other.data.unit_spawn;
			NEW( data.unit_spawn.unitdef_id, std::string, *other.data.unit_spawn.unitdef_id );
			break;
		}
		case FR_UNIT_DESPAWN: {
			data.unit_despawn.unit_id = other.data.unit_despawn.unit_id;
			break;
		}
		case FR_UNIT_REFRESH: {
			data.unit_refresh = other.data.unit_refresh;
			break;
		}
		case FR_UNIT_MOVE: {
			data.unit_move = other.data.unit_move;
			break;
		}

		default: {
			//
		}
	}
}

FrontendRequest::~FrontendRequest() {
	switch ( type ) {
		case FR_QUIT: {
			DELETE( data.quit.reason );
			break;
		}
		case FR_ERROR: {
			DELETE( data.error.what );
			DELETE( data.error.stacktrace );
			break;
		}
		case FR_GLOBAL_MESSAGE: {
			DELETE( data.global_message.message );
			break;
		}
		case FR_SLOT_DEFINE: {
			DELETE( data.slot_define.slotdefs );
			break;
		}
		case FR_UNIT_DEFINE: {
			DELETE( data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_SPAWN: {
			DELETE( data.unit_spawn.unitdef_id );
			break;
		}
		default: {
			//
		}
	}
}

}
