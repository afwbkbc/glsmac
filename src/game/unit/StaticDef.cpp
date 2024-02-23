#include "StaticDef.h"

namespace game {
namespace unit {

StaticDef::StaticDef( const std::string& id, const std::string& name, const Render* render )
	: Def( id, DT_STATIC, name )
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
		TS_OBJ_PROP( "render", m_render->ToString( TS_PREFIX_NEXT ) ) +
		TS_OBJ_END();
}

void StaticDef::Serialize( types::Buffer& buf, const StaticDef* def ) {
	Render::Serialize( buf, def->m_render );
}

StaticDef* StaticDef::Unserialize( types::Buffer& buf, const std::string& id, const std::string& name ) {
	return new StaticDef( id, name, Render::Unserialize( buf ) );
}

}
}
