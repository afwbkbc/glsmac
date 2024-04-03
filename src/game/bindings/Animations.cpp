#include "Binding.h"

#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"

#include "game/event/DefineAnimation.h"
#include "game/animation/FramesRow.h"

namespace game {
namespace bindings {

BINDING_IMPL( animations ) {
	const gse::type::Object::properties_t properties = {
		{
			"define",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( animation_def, 1, Object );
				N_GETPROP( type, animation_def, "type", String );

				if ( type == "frames_row" ) {
					N_GETPROP( file, animation_def, "file", String );
					N_GETPROP( row_x, animation_def, "row_x", Int );
					N_GETPROP( row_y, animation_def, "row_y", Int );
					N_GETPROP( frame_width, animation_def, "frame_width", Int );
					N_GETPROP( frame_height, animation_def, "frame_height", Int );
					N_GETPROP( frame_padding, animation_def, "frame_padding", Int );
					N_GETPROP( frames_count, animation_def, "frames_count", Int );
					N_GETPROP( duration_ms, animation_def, "duration_ms", Int );
					N_GETPROP( sound, animation_def, "sound", String );
					auto* def = new animation::FramesRow( id, file, row_x, row_y, frame_width, frame_height, frame_padding, frames_count, duration_ms, sound );
					auto* game = GAME;
					game->AddEvent( new event::DefineAnimation( game->GetSlotNum(), def ) );
					return VALUE( gse::type::Undefined );
				}
				else {
					ERROR( gse::EC.GAME_ERROR, "Unsupported animation type: " + type );
				}
				return VALUE( gse::type::Undefined );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
