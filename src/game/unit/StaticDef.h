#pragma once

#include "Def.h"

#include "Render.h"

#include "Unit.h"

namespace game {
namespace unit {

class StaticDef : public Def {
public:
	static const std::string& GetMovementTypeString( const movement_type_t movement_type );

	StaticDef( const std::string& id, const std::string& name, const movement_type_t movement_type, const Unit::movement_t movement_per_turn, const Render* render );
	~StaticDef();

	const movement_type_t m_movement_type;
	const Unit::movement_t m_movement_per_turn;
	const Render* m_render;

	const movement_type_t GetMovementType() const override;

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
