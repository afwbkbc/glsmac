#include "Binding.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"
#include "gse/type/Undefined.h"
#include "game/backend/Game.h"
#include "Bindings.h"
#include "game/backend/event/DefineAnimation.h"
#include "game/backend/animation/FramesRow.h"
#include "engine/Engine.h"
#include "loader/sound/SoundLoader.h"

namespace game {
namespace backend {
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
					N_GETPROP_OPT( int64_t, frame_center_x, animation_def, "frame_center_x", Int, frame_width / 2 );
					N_GETPROP_OPT( int64_t, frame_center_y, animation_def, "frame_center_y", Int, frame_height / 2 );
					N_GETPROP( frame_padding, animation_def, "frame_padding", Int );
					N_GETPROP( frames_count, animation_def, "frames_count", Int );
					N_GETPROP_OPT( int64_t, frames_per_row, animation_def, "frames_per_row", Int, frames_count );
					N_GETPROP_OPT( float, scale_x, animation_def, "scale_x", Float, 1.0f );
					N_GETPROP_OPT( float, scale_y, animation_def, "scale_y", Float, 1.0f );
					N_GETPROP( duration_ms, animation_def, "duration_ms", Int );
					N_GETPROP( sound, animation_def, "sound", String );
					if ( !g_engine->GetSoundLoader()->LoadCustomSound( sound ) ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Failed to load animation sound '" + sound + "'" );
					}
					auto* def = new animation::FramesRow(
						id,
						file,
						row_x,
						row_y,
						frame_width,
						frame_height,
						frame_center_x,
						frame_center_y,
						frame_padding,
						frames_count,
						frames_per_row,
						scale_x,
						scale_y,
						duration_ms,
						sound
					);
					auto* game = GAME;
					return game->AddEvent( new event::DefineAnimation( game->GetSlotNum(), def ) );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported animation type: " + type );
				}
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
					GSE_ERROR( gse::EC.GAME_ERROR, *errmsg );
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
}
