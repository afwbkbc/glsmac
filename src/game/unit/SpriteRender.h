#pragma once

#include <string>

#include "Render.h"

namespace game {
namespace unit {

class SpriteRender : public Render {
public:
	SpriteRender(
		const std::string& file,
		const uint32_t x,
		const uint32_t y,
		const uint32_t w,
		const uint32_t h,
		const uint32_t cx,
		const uint32_t cy,
		const uint32_t morale_based_xshift
	);

	const std::string m_file;
	const uint32_t m_x;
	const uint32_t m_y;
	const uint32_t m_w;
	const uint32_t m_h;
	const uint32_t m_cx;
	const uint32_t m_cy;
	const uint32_t m_morale_based_xshift;
	const float m_cshift_x;
	const float m_cshift_y;

	const types::Vec3 GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const override;
	const std::string ToString( const std::string& prefix ) const override;

private:
	friend class Render;

	static void Serialize( types::Buffer& buf, const SpriteRender* render );
	static SpriteRender* Unserialize( types::Buffer& buf );

};

}
}
