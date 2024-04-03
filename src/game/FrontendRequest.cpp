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

	data = other.data;

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
		case FR_SLOT_DEFINE: {
			NEW( data.slot_define.slotdefs, slot_defines_t, *other.data.slot_define.slotdefs );
			break;
		}
		case FR_ANIMATION_DEFINE: {
			NEW( data.animation_define.serialized_animation, std::string, *other.data.animation_define.serialized_animation );
			break;
		}
		case FR_UNIT_DEFINE: {
			NEW( data.unit_define.serialized_unitdef, std::string, *other.data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_SPAWN: {
			NEW( data.unit_spawn.unitdef_id, std::string, *other.data.unit_spawn.unitdef_id );
			NEW( data.unit_spawn.morale_string, std::string, *other.data.unit_spawn.morale_string );
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
		case FR_ANIMATION_DEFINE: {
			DELETE( data.animation_define.serialized_animation );
			break;
		}
		case FR_UNIT_DEFINE: {
			DELETE( data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_SPAWN: {
			DELETE( data.unit_spawn.unitdef_id );
			DELETE( data.unit_spawn.morale_string );
			break;
		}
		default: {
			//
		}
	}
}

}
