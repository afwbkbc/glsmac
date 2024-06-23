#include "Binding.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Object.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/Undefined.h"
#include "gse/type/Bool.h"
#include "game/bindings/Bindings.h"
#include "game/State.h"

#include "types/Color.h"

namespace game {
namespace bindings {

BINDING_IMPL( factions ) {
	auto& factions = m_bindings->GetState()->m_settings.global.game_rules.m_factions;
	auto& factions_order = m_bindings->GetState()->m_settings.global.game_rules.m_factions_order;
	const gse::type::object_properties_t properties = {
		{
			"define",
			NATIVE_CALL( this, &factions, &factions_order ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				if ( factions.find( id ) != factions.end() ) {
					ERROR( gse::EC.GAME_ERROR, "Faction '" + id + "' already exists" );
				}
				N_GETVALUE( faction_def, 1, Object );
				N_GETPROP( name, faction_def, "name", String );

				rules::Faction faction = { id, name };

				N_GETPROP( colors, faction_def, "colors", Object );
				N_GETPROP_UNWRAP( colors_text, colors, "text", types::Color );
				N_GETPROP_UNWRAP( colors_border, colors, "border", types::Color );
				faction.m_colors = { colors_text, colors_border };

				N_GETPROP_OPT_BOOL( is_naval, faction_def, "is_naval")
				if ( is_naval ) {
					faction.m_flags |= rules::Faction::FF_NAVAL;
				}
				N_GETPROP_OPT_BOOL( is_progenitor, faction_def, "is_progenitor")
				if ( is_progenitor ) {
					faction.m_flags |= rules::Faction::FF_PROGENITOR;
				}

				N_GETPROP( bases_def, faction_def, "bases", Object );
				N_GETPROP( bases_render_def, bases_def, "render", Object );

				N_GETPROP( bases_render_type, bases_render_def, "type", String );
				if ( bases_render_type == "sprite_grid" ) {
					N_GETPROP( file, bases_render_def, "file", String );
					N_GETPROP( grid_x, bases_render_def, "grid_x", Int );
					N_GETPROP( grid_y, bases_render_def, "grid_y", Int );
					N_GETPROP( cell_width, bases_render_def, "cell_width", Int );
					N_GETPROP( cell_height, bases_render_def, "cell_height", Int );
					N_GETPROP_OPT( size_t, cell_cx, bases_render_def, "cell_cx", Int, cell_width / 2 );
					N_GETPROP_OPT( size_t, cell_cy, bases_render_def, "cell_cy", Int, cell_height / 2 );
					N_GETPROP( cell_padding, bases_render_def, "cell_padding", Int );
					N_GETPROP_OPT( float, scale_x, bases_render_def, "scale_x", Float, 1.0f );
					N_GETPROP_OPT( float, scale_y, bases_render_def, "scale_y", Float, 1.0f );
					faction.m_bases_render = {
						file,
						(size_t)grid_x,
						(size_t)grid_y,
						(size_t)cell_width,
						(size_t)cell_height,
						cell_cx,
						cell_cy,
						(size_t)cell_padding,
						scale_x,
						scale_y
					};
				}
				else {
					ERROR( gse::EC.GAME_ERROR, "Unsupported bases render type: " + bases_render_type );
				}

				factions.insert({ id, faction });
				factions_order.push_back( id );
				return VALUE( gse::type::Undefined );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
