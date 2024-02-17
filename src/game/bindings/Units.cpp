#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"

#include "game/unit/StaticDef.h"
#include "game/unit/SpriteRender.h"

#include "game/event/DefineUnit.h"
#include "game/event/SpawnUnit.h"
#include "game/event/DespawnUnit.h"

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
				N_GETPROP( unit_type, unit_def, "type", String );
				if ( unit_type == "static" ) {
					N_GETPROP( render_def, unit_def, "render", Object );
					N_GETPROP( render_type, render_def, "type", String );
					if ( render_type == "sprite" ) {
						N_GETPROP( sprite_file, render_def, "file", String );
						N_GETPROP( sprite_x, render_def, "x", Int );
						N_GETPROP( sprite_y, render_def, "y", Int );
						N_GETPROP( sprite_w, render_def, "w", Int );
						N_GETPROP( sprite_h, render_def, "h", Int );
						N_GETPROP( sprite_cx, render_def, "cx", Int );
						N_GETPROP( sprite_cy, render_def, "cy", Int );
						const auto* def = new unit::StaticDef(
							name,
							new unit::SpriteRender( sprite_file, sprite_x, sprite_y, sprite_w, sprite_h, sprite_cx, sprite_cy )
						);
						return GAME->AddGameEvent( new event::DefineUnit( def ), ctx, call_si );
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
				N_EXPECT_ARGS( 3 );
				N_GETVALUE( def_name, 0, String );
				N_UNWRAP( owner, 1, Slot );
				N_UNWRAP( tile, 2, map::Tile );
				return GAME->AddGameEvent( new event::SpawnUnit( def_name, owner->GetIndex(), tile->coord.x, tile->coord.y ), ctx, call_si );
			})
		},
		{
			"despawn",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_UNWRAP( unit, 0, unit::Unit );
				return GAME->AddGameEvent( new event::DespawnUnit( unit->m_id ), ctx, call_si );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
