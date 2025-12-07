#pragma once

#include "Render.h"

#include "Types.h"

namespace game {
namespace backend {
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

	const sprite_render_info_t m_render = {};

	const std::string ToString( const std::string& prefix ) const override;

private:
	friend class Render;

	static void Serialize( types::Buffer& buf, const SpriteRender* render );
	static SpriteRender* Deserialize( types::Buffer& buf );

};

}
}
}
