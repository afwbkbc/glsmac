#pragma once

#include <string>

#include "Render.h"

namespace game {
namespace unit {

class SpriteRender : public Render {
public:
	SpriteRender( const std::string& file, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h );

	const std::string m_file;
	const uint32_t m_x;
	const uint32_t m_y;
	const uint32_t m_w;
	const uint32_t m_h;

private:
	friend class Render;

	static void Serialize( types::Buffer& buf, const SpriteRender* render );
	static SpriteRender* Unserialize( types::Buffer& buf );

};

}
}
