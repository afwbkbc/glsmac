#include "Factions.h"

#include "Faction.h"

#include "gse/callable/Native.h"
#include "gse/type/Bool.h"
#include "gse/type/Float.h"
#include "gse/type/Array.h"

namespace game {
namespace backend {

Factions::Factions() {
	//
}

Factions::~Factions() {
	Clear();
}

void Factions::Add( Faction* faction ) {
	Remove( faction->m_id );
	m_factions.insert( { faction->m_id, { faction, ++m_next_faction_idx } } );
	m_factions_order.insert({ m_next_faction_idx, faction->m_id });
}

void Factions::Remove( const std::string& id ) {
	const auto& it = m_factions.find( id );
	if ( it != m_factions.end() ) {
		delete it->second.faction;
		m_factions_order.erase( it->second.index );
		m_factions.erase( it );
	}
}

void Factions::Clear() {
	for ( const auto& it : m_factions ) {
		delete it.second.faction;
	}
	m_factions.clear();
	m_factions_order.clear();
	m_next_faction_idx = 0;
}

Faction* Factions::Get( const std::string& id ) const {
	const auto& it = m_factions.find( id );
	if ( it != m_factions.end() ) {
		return it->second.faction;
	}
	return nullptr;
}

const std::vector< Faction* > Factions::GetAll() const {
	ASSERT_NOLOG( m_factions.size() == m_factions_order.size(), "factions order size mismatch" );
	std::vector< Faction* > result = {};
	result.reserve( m_factions.size() );
	for ( const auto& it : m_factions_order ) {
		ASSERT_NOLOG( m_factions.find( it.second ) != m_factions.end(), "faction not found" );
		result.push_back( m_factions.at( it.second ).faction );
	}
	return result;
}

WRAPIMPL_BEGIN( Factions, CLASS_FACTIONS )
	WRAPIMPL_PROPS
		{
			"add",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );

				N_GETVALUE( faction_def, 1, Object );
				N_GETPROP( name, faction_def, "name", String );

				auto* faction = new Faction( id, name );

				N_GETPROP( colors, faction_def, "colors", Object );
				N_GETPROP_UNWRAP( colors_text, colors, "text", types::Color );
				N_GETPROP_UNWRAP( colors_text_shadow, colors, "text_shadow", types::Color );
				N_GETPROP_UNWRAP( colors_border, colors, "border", types::Color );
				faction->m_colors = {
					colors_text,
					colors_text_shadow,
					colors_border
				};

				N_GETPROP_OPT_BOOL( is_naval, faction_def, "is_naval")
				if ( is_naval ) {
					faction->m_flags |= Faction::FF_NAVAL;
				}
				N_GETPROP_OPT_BOOL( is_progenitor, faction_def, "is_progenitor")
				if ( is_progenitor ) {
					faction->m_flags |= Faction::FF_PROGENITOR;
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
					faction->m_bases_render = {
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
					delete faction;
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported bases render type: " + bases_render_type );
				}

				N_GETPROP( base_names, bases_def, "names", Object );
				N_GETPROP( base_names_land, base_names, "land", Array );
				faction->m_base_names.land.reserve( base_names_land.size() );
				for ( size_t i = 0  ; i < base_names_land.size() ; i++ ) {
					N_GETELEMENT( v, base_names_land, i, String );
					faction->m_base_names.land.push_back( v );
				}

				N_GETPROP( base_names_water, base_names, "water", Array );
				faction->m_base_names.water.reserve( base_names_water.size() );
				for ( size_t i = 0  ; i < base_names_water.size() ; i++ ) {
					N_GETELEMENT( v, base_names_water, i, String );
					faction->m_base_names.water.push_back( v );
				}

				Add( faction );
				return faction->Wrap();
			} )
		},
	};
WRAPIMPL_END_PTR( Factions )

UNWRAPIMPL_PTR( Factions )

const types::Buffer Factions::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_factions_order.size() );
	for ( auto& it : m_factions_order ) {
		const auto* faction = m_factions.at( it.second ).faction;
		buf.WriteString( it.second );
		buf.WriteString( faction->Serialize().ToString() );
	}

	return buf;
}

void Factions::Unserialize( types::Buffer buf ) {
	Clear();

	const size_t factions_count = buf.ReadInt();
	for ( size_t i = 0 ; i < factions_count ; i++ ) {
		const auto faction_id = buf.ReadString();
		ASSERT_NOLOG( m_factions.find( faction_id ) == m_factions.end(), "duplicate faction id" );
		m_factions.insert({ faction_id, { new Faction(), ++m_next_faction_idx }}).first->second.faction->Unserialize( buf.ReadString() );
		m_factions_order.insert( { m_next_faction_idx, faction_id } );
	}
}


}
}
