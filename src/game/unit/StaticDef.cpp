#include "StaticDef.h"

namespace game {
namespace unit {

const std::unordered_map< StaticDef::movement_type_t, std::string > StaticDef::s_movement_type_str = {
	{
		MT_IMMOVABLE,
		"IMMOVABLE"
	},
	{
		MT_LAND,
		"LAND"
	},
	{
		MT_SEA,
		"SEA"
	},
	{
		MT_AIR,
		"AIR"
	}
};
const std::string& StaticDef::GetMovementTypeString( const movement_type_t movement_type ) {
	const auto& it = s_movement_type_str.find( movement_type );
	ASSERT_NOLOG( it != s_movement_type_str.end(), "unknown movement type: " + std::to_string( movement_type ) );
	return it->second;
}

StaticDef::StaticDef( const std::string& id, const std::string& name, const movement_type_t movement_type, const Render* render )
	: Def( id, DT_STATIC, name )
	, m_movement_type( movement_type )
	, m_render( render ) {}

StaticDef::~StaticDef() {
	delete m_render;
}

const types::Vec3 StaticDef::GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const {
	return m_render->GetSpawnCoords( tile_x, tile_y, tile_coords );
}

const std::string StaticDef::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "StaticDef" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		TS_OBJ_PROP_STR( "movement_type", GetMovementTypeString( m_movement_type ) ) +
		TS_OBJ_PROP( "render", m_render->ToString( TS_PREFIX_NEXT ) ) +
		TS_OBJ_END();
}

void StaticDef::Serialize( types::Buffer& buf, const StaticDef* def ) {
	buf.WriteInt( def->m_movement_type );
	Render::Serialize( buf, def->m_render );
}

StaticDef* StaticDef::Unserialize( types::Buffer& buf, const std::string& id, const std::string& name ) {
	const auto movement_type = (movement_type_t)buf.ReadInt();
	return new StaticDef( id, name, movement_type, Render::Unserialize( buf ) );
}

}
}
