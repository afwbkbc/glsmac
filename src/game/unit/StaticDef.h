#pragma once

#include "Def.h"

#include "Render.h"

namespace game {
namespace unit {

class StaticDef : public Def {
public:
	enum movement_type_t {
		MT_IMMOVABLE,
		MT_LAND,
		MT_SEA,
		MT_AIR
	};
	static const std::string& GetMovementTypeString( const movement_type_t movement_type );

	StaticDef( const std::string& id, const std::string& name, const movement_type_t movement_type, const Render* render );
	~StaticDef();

	const movement_type_t m_movement_type;
	const Render* m_render;

	const types::Vec3 GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const override;
	const std::string ToString( const std::string& prefix ) const override;

private:
	static const std::unordered_map< movement_type_t, std::string > s_movement_type_str;

private:
	friend class Def;

	static void Serialize( types::Buffer& buf, const StaticDef* def );
	static StaticDef* Unserialize( types::Buffer& buf, const std::string& id, const std::string& name );

};

}
}
