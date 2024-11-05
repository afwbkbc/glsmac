#include "Binding.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Object.h"
#include "gse/type/Array.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/Undefined.h"
#include "gse/type/Bool.h"
#include "Bindings.h"
#include "game/backend/State.h"
#include "engine/Engine.h"
#include "loader/txt/TXTLoaders.h"
#include "loader/txt/FactionTXTLoader.h"
#include "loader/texture/TextureLoader.h"
#include "types/texture/Texture.h"

#include "types/Color.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( factions ) {
	auto& factions = m_bindings->GetState()->m_settings.global.game_rules.m_factions;
	auto& factions_order = m_bindings->GetState()->m_settings.global.game_rules.m_factions_order;
	const gse::type::object_properties_t properties = {
		{
			"import_base_names",
			NATIVE_CALL() {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( filename, 0, String );
				const auto& data = g_engine->GetTXTLoaders()->factions->GetFactionData( filename );
				std::vector< gse::Value > land_names = {};
				land_names.reserve( data.bases_names.land.size() );
				for ( const auto& name : data.bases_names.land ) {
					land_names.push_back( VALUE( gse::type::String, name ) );
				}
				std::vector< gse::Value > water_names = {};
				water_names.reserve( data.bases_names.water.size() );
				for ( const auto& name : data.bases_names.water ) {
					water_names.push_back( VALUE( gse::type::String, name ) );
				}
				const auto properties = gse::type::object_properties_t{
					{ "land", VALUE( gse::type::Array, land_names ) },
					{ "water", VALUE( gse::type::Array, water_names ) },
				};
				return VALUE( gse::type::Object, properties );
			} )
		},
		{
			"import_colors",
			NATIVE_CALL() {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( filename, 0, String );
				const auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( filename );
				const auto properties = gse::type::object_properties_t{
					{ "faction", types::Color::FromRGBA( texture->GetPixel( 4, 739 ) ).Wrap() },
					{ "faction_shadow", types::Color::FromRGBA( texture->GetPixel( 4, 747 ) ).Wrap() },
					{ "text", types::Color::FromRGBA( texture->GetPixel( 4, 755 ) ).Wrap() },
					{ "text_shadow", types::Color::FromRGBA( texture->GetPixel( 4, 763 ) ).Wrap() },
					{ "border", types::Color::FromRGBA( texture->GetPixel( 161, 749 ) ).Wrap() },
					{ "border_alpha", types::Color::FromRGBA( texture->GetPixel( 161, 757 ) ).Wrap() },
					{ "vehicle", types::Color::FromRGBA( texture->GetPixel( 435, 744 ) ).Wrap() },
				};
				return VALUE( gse::type::Object, properties );
			} )
		},
		{
			"define",
			NATIVE_CALL( &factions, &factions_order ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				if ( factions.find( id ) != factions.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Faction '" + id + "' already exists" );
				}
				N_GETVALUE( faction_def, 1, Object );
				N_GETPROP( name, faction_def, "name", String );

				rules::Faction faction = { id, name };

				N_GETPROP( colors, faction_def, "colors", Object );
				N_GETPROP_UNWRAP( colors_text, colors, "text", types::Color );
				N_GETPROP_UNWRAP( colors_text_shadow, colors, "text_shadow", types::Color );
				N_GETPROP_UNWRAP( colors_border, colors, "border", types::Color );
				faction.m_colors = {
					colors_text,
					colors_text_shadow,
					colors_border
				};

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
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported bases render type: " + bases_render_type );
				}

				N_GETPROP( base_names, bases_def, "names", Object );
				N_GETPROP( base_names_land, base_names, "land", Array );
				faction.m_base_names.land.reserve( base_names_land.size() );
				for ( size_t i = 0  ; i < base_names_land.size() ; i++ ) {
					N_GETELEMENT( v, base_names_land, i, String );
					faction.m_base_names.land.push_back( v );
				}

				N_GETPROP( base_names_water, base_names, "water", Array );
				faction.m_base_names.water.reserve( base_names_water.size() );
				for ( size_t i = 0  ; i < base_names_water.size() ; i++ ) {
					N_GETELEMENT( v, base_names_water, i, String );
					faction.m_base_names.water.push_back( v );
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
}
