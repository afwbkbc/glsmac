#include "FrontendRequest.h"

#include <cstring>

#include "common/Common.h"

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
			if ( other.data.error.stacktrace ) {
				NEW( data.error.stacktrace, std::string, *other.data.error.stacktrace );
			}
			break;
		}
		case FR_GLOBAL_MESSAGE: {
			NEW( data.global_message.message, std::string, *other.data.global_message.message );
			break;
		}
		case FR_UPDATE_TILES: {
			NEW( data.update_tiles.tile_updates, tile_updates_t, *other.data.update_tiles.tile_updates );
			break;
		}
		case FR_FACTION_DEFINE: {
			NEW( data.faction_define.factiondefs, faction_defines_t, *other.data.faction_define.factiondefs );
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
		case FR_ANIMATION_UNDEFINE: {
			NEW( data.animation_undefine.animation_id, std::string, *other.data.animation_undefine.animation_id );
			break;
		}
		case FR_ANIMATION_SHOW: {
			NEW( data.animation_show.animation_id, std::string, *other.data.animation_show.animation_id );
			break;
		}
		case FR_UNIT_DEFINE: {
			NEW( data.unit_define.serialized_unitdef, std::string, *other.data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_UNDEFINE: {
			NEW( data.unit_undefine.id, std::string, *other.data.unit_undefine.id );
			break;
		}
		case FR_UNIT_SPAWN: {
			NEW( data.unit_spawn.unitdef_id, std::string, *other.data.unit_spawn.unitdef_id );
			NEW( data.unit_spawn.morale_string, std::string, *other.data.unit_spawn.morale_string );
			break;
		}
		case FR_BASE_POP_DEFINE: {
			NEW( data.base_pop_define.serialized_popdef, std::string, *other.data.base_pop_define.serialized_popdef );
			break;
		}
		case FR_BASE_SPAWN: {
			NEW( data.base_spawn.name, std::string, *other.data.base_spawn.name );
			break;
		}
		case FR_BASE_UPDATE: {
			NEW( data.base_update.name, std::string, *other.data.base_update.name );
			NEW( data.base_update.faction_id, std::string, *other.data.base_update.faction_id );
			NEW( data.base_update.pops, base_pops_t, *other.data.base_update.pops );
			break;
		}
		case FR_LOADER_SHOW:
		case FR_LOADER_TEXT: {
			NEW( data.loader.text, std::string, *other.data.loader.text );
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
			if ( data.error.stacktrace ) {
				DELETE( data.error.stacktrace );
			}
			break;
		}
		case FR_GLOBAL_MESSAGE: {
			DELETE( data.global_message.message );
			break;
		}
		case FR_UPDATE_TILES: {
			DELETE( data.update_tiles.tile_updates );
			break;
		}
		case FR_FACTION_DEFINE: {
			DELETE( data.faction_define.factiondefs );
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
		case FR_ANIMATION_UNDEFINE: {
			DELETE( data.animation_undefine.animation_id );
			break;
		}
		case FR_ANIMATION_SHOW: {
			DELETE( data.animation_show.animation_id );
			break;
		}
		case FR_UNIT_DEFINE: {
			DELETE( data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_UNDEFINE: {
			DELETE( data.unit_undefine.id );
			break;
		}
		case FR_UNIT_SPAWN: {
			DELETE( data.unit_spawn.unitdef_id );
			DELETE( data.unit_spawn.morale_string );
			break;
		}
		case FR_BASE_POP_DEFINE: {
			DELETE( data.base_pop_define.serialized_popdef );
			break;
		}
		case FR_BASE_SPAWN: {
			DELETE( data.base_spawn.name );
			break;
		}
		case FR_BASE_UPDATE: {
			DELETE( data.base_update.name );
			DELETE( data.base_update.faction_id );
			DELETE( data.base_update.pops );
			break;
		}
		case FR_LOADER_TEXT:
		case FR_LOADER_SHOW: {
			DELETE( data.loader.text );
			break;
		}
		default: {
			//
		}
	}
}

}
