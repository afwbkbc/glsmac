#include "Faction.h"

#include <cmath>
#include <unordered_set>
#include <utility>

#include "types/texture/Texture.h"
#include "gse/value/String.h"
#include "gse/value/Bool.h"
#include "gse/value/Array.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "resource/ResourceManager.h"
#include "util/String.h"

namespace game {
namespace backend {
namespace faction {

Faction::Faction() {
	//
}

Faction::Faction( const std::string& id, const std::string& name )
	: m_id( id )
	, m_name( name ) {
	//
}

const types::Buffer Faction::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_id );
	buf.WriteString( m_name );
	buf.WriteInt( m_flags );

	buf.WriteColor( m_colors.text );
	buf.WriteColor( m_colors.text_shadow );
	buf.WriteColor( m_colors.border );

	buf.WriteString( m_bases_render.file );
	buf.WriteInt( m_bases_render.grid_x );
	buf.WriteInt( m_bases_render.grid_y );
	buf.WriteInt( m_bases_render.cell_width );
	buf.WriteInt( m_bases_render.cell_height );
	buf.WriteInt( m_bases_render.cell_cx );
	buf.WriteInt( m_bases_render.cell_cy );
	buf.WriteInt( m_bases_render.cell_padding );
	buf.WriteFloat( m_bases_render.scale_x );
	buf.WriteFloat( m_bases_render.scale_y );

	buf.WriteInt( m_base_names.land.size() );
	for ( const auto& name : m_base_names.land ) {
		buf.WriteString( name );
	}
	buf.WriteInt( m_base_names.water.size() );
	for ( const auto& name : m_base_names.water ) {
		buf.WriteString( name );
	}
	buf.WriteInt( m_starting_technologies.size() );
	for ( const auto& id : m_starting_technologies ) {
		buf.WriteString( id );
	}

	return buf;
}

void Faction::Deserialize( types::Buffer buf ) {

	const auto id = buf.ReadString();
	const auto name = buf.ReadString();
	const auto flags = buf.ReadInt< faction_flag_t >( "faction flags" );
	if ( flags > FF_ALL ) {
		THROW( "invalid serialized faction flags: " + std::to_string( flags ) );
	}

	types::Color text;
	types::Color text_shadow;
	types::Color border;
	buf.ReadColor( text );
	buf.ReadColor( text_shadow );
	buf.ReadColor( border );

	bases_render_info_t bases_render = {};
	bases_render.file = buf.ReadString();
	bases_render.grid_x = buf.ReadInt< size_t >( "faction base render grid x" );
	bases_render.grid_y = buf.ReadInt< size_t >( "faction base render grid y" );
	bases_render.cell_width = buf.ReadInt< size_t >( "faction base render cell width" );
	bases_render.cell_height = buf.ReadInt< size_t >( "faction base render cell height" );
	bases_render.cell_cx = buf.ReadInt< size_t >( "faction base render center x" );
	bases_render.cell_cy = buf.ReadInt< size_t >( "faction base render center y" );
	bases_render.cell_padding = buf.ReadInt< size_t >( "faction base render padding" );
	bases_render.scale_x = buf.ReadFloat();
	bases_render.scale_y = buf.ReadFloat();
	if ( !std::isfinite( bases_render.scale_x ) || !std::isfinite( bases_render.scale_y ) ) {
		THROW( "invalid serialized faction base render scale" );
	}

	std::vector< std::string > land_names;
	const auto land_names_count = buf.ReadCollectionSize( "faction land base name" );
	land_names.reserve( land_names_count );
	for ( size_t i = 0 ; i < land_names_count ; i++ ) {
		land_names.push_back( buf.ReadString() );
	}
	std::vector< std::string > water_names;
	const auto water_names_count = buf.ReadCollectionSize( "faction water base name" );
	water_names.reserve( water_names_count );
	for ( size_t i = 0 ; i < water_names_count ; i++ ) {
		water_names.push_back( buf.ReadString() );
	}
	std::vector< std::string > starting_technologies = {};
	std::unordered_set< std::string > technology_ids = {};
	const auto technology_count = buf.ReadCollectionSize( "faction starting technology" );
	starting_technologies.reserve( technology_count );
	for ( size_t i = 0 ; i < technology_count ; i++ ) {
		const auto id = buf.ReadString();
		if ( id.empty() || !technology_ids.insert( id ).second ) {
			THROW( "invalid or duplicate serialized faction starting technology" );
		}
		starting_technologies.push_back( id );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized faction" );
	}

	m_id = id;
	m_name = name;
	m_flags = flags;
	m_colors = { text, text_shadow, border };
	m_bases_render = bases_render;
	m_base_names.land = std::move( land_names );
	m_base_names.water = std::move( water_names );
	m_starting_technologies = std::move( starting_technologies );

}

WRAPIMPL_BEGIN( Faction )
	WRAPIMPL_PROPS
			{
				"id",
				VALUE( gse::value::String, , m_id )
			},
			{
				"name",
				VALUE( gse::value::String, , m_name )
			},
			{
				"text_color",
				m_colors.text.Wrap( GSE_CALL ),
			},
			{
				"is_naval",
				VALUE( gse::value::Bool, , m_flags & Faction::FF_NAVAL )
			},
			{
				"is_progenitor",
				VALUE( gse::value::Bool, , m_flags & Faction::FF_PROGENITOR )
			},
			{
				"is_native",
				VALUE( gse::value::Bool, , m_flags & Faction::FF_NATIVE )
			},
			{
				"get_starting_technologies",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					gse::value::array_elements_t result = {};
					result.reserve( m_starting_technologies.size() );
					for ( const auto& id : m_starting_technologies ) {
						result.push_back( VALUE( gse::value::String, , id ) );
					}
					return VALUE( gse::value::Array, , result );
				} )
			},
		};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Faction )

}
}
}
