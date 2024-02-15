#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"

#include "../event/SpawnUnit.h"
#include "game/unit/StaticDef.h"
#include "game/unit/SpriteRender.h"

namespace game {
namespace bindings {

BINDING_IMPL( units ) {
	const gse::type::Object::properties_t properties = {
		{
			"define",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( name, 0, String );
				N_GETVALUE( unit_def, 1, Object );
				N_GETPROP( unit_def, unit_type, "type", String );
				if ( unit_type == "static" ) {
					N_GETPROP( unit_def, render_def, "render", Object );
					N_GETPROP( render_def, render_type, "type", String );
					if ( render_type == "sprite" ) {
						N_GETPROP( render_def, sprite_file, "file", String );
						N_GETPROP( render_def, sprite_x, "x", Int );
						N_GETPROP( render_def, sprite_y, "y", Int );
						N_GETPROP( render_def, sprite_w, "w", Int );
						N_GETPROP( render_def, sprite_h, "h", Int );
						N_GETPROP( render_def, sprite_cx, "cx", Int );
						N_GETPROP( render_def, sprite_cy, "cy", Int );
						const auto* def = new unit::StaticDef(
							name,
							new unit::SpriteRender( sprite_file, sprite_x, sprite_y, sprite_w, sprite_h, sprite_cx, sprite_cy )
						);
						m_game->AddUnitDef( name, def, ctx, call_si );
						return def->Wrap();
					}
					else {
						ERROR( gse::EC.GAME_ERROR, "Unsupported render type: " + render_type );
					}
				}
				else {
					ERROR( gse::EC.GAME_ERROR, "Unsupported unit type: " + unit_type );
				}
				return VALUE( gse::type::Undefined );
			})
		},
		{
			"spawn",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( def, 0, String );
				N_UNWRAP( tile, 1, map::Tile );
				return m_game->AddGameEvent( new event::SpawnUnit( def, tile->coord.x, tile->coord.y ), ctx, call_si );
			})
		}
	};
	return VALUE( gse::type::Object, properties );
}

}
}
