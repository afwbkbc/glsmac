#include "Binding.h"

#include "gse/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"
#include "gse/type/Undefined.h"
#include "game/Game.h"
#include "game/bindings/Bindings.h"
#include "game/event/DefineAnimation.h"
#include "game/animation/FramesRow.h"
#include "engine/Engine.h"
#include "loader/sound/SoundLoader.h"

namespace game {
namespace bindings {

BINDING_IMPL( animations ) {
	const gse::type::object_properties_t properties = {
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
					if ( !g_engine->GetSoundLoader()->LoadSound( sound ) ) {
						ERROR( gse::EC.GAME_ERROR, "Failed to load animation sound '" + sound + "'" );
					}
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
		{
			"show_on_tile",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 3 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );
				N_PERSIST_CALLABLE( on_complete, 2 );
				const auto* errmsg = GAME->ShowAnimationOnTile( id, tile, [ on_complete, ctx, call_si ]() {
					on_complete->Run( ctx, call_si, {} );
					N_UNPERSIST_CALLABLE( on_complete );
				});
				if ( errmsg ) {
					ERROR( gse::EC.GAME_ERROR, *errmsg );
					delete errmsg;
				}
				return VALUE( gse::type::Undefined );
			} )
		}
	};
	return VALUE( gse::type::Object, properties );
}

}
}
