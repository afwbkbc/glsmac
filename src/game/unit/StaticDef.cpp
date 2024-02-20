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

void StaticDef::Serialize( types::Buffer& buf, const StaticDef* def ) {
	Render::Serialize( buf, def->m_render );
}

StaticDef* StaticDef::Unserialize( types::Buffer& buf, const std::string& id, const std::string& name ) {
	return new StaticDef( id, name, Render::Unserialize( buf ) );
}

}
}
