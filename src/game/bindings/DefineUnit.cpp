#include "Binding.h"

#include "gse/type/String.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"

#include "../unit/StaticDef.h"
#include "../unit/SpriteRender.h"

namespace game {
namespace bindings {

BINDING_IMPL( define_unit ) {
	return NATIVE_CALL( this ) {
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
				m_game->AddUnitDef( name,
					new unit::StaticDef(
						name,
						new unit::SpriteRender( sprite_file, sprite_x, sprite_y, sprite_w, sprite_h )
					),
				ctx, call_si );
			}
			else {
				ERROR( "Unsupported render type: " + render_type );
			}
		}
		else {
			ERROR( "Unsupported unit type: " + unit_type );
		}
		return VALUE( gse::type::Undefined );
	});
}

}
}
