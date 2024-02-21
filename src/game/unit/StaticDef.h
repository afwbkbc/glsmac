#pragma once

#include "Def.h"

#include "Render.h"

namespace game {
namespace unit {

class StaticDef : public Def {
public:
	StaticDef( const std::string& id, const std::string& name, const Render* render );
	~StaticDef();

	const Render* m_render;

	const types::Vec3 GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const override;

private:
	friend class Def;

	static void Serialize( types::Buffer& buf, const StaticDef* def );
	static StaticDef* Unserialize( types::Buffer& buf, const std::string& id, const std::string& name );

};

}
}
