#pragma once

#include <string>

#include "Render.h"

namespace game {
namespace unit {

class SpriteRender : public Render {
public:
	SpriteRender( const std::string& file, const size_t x, const size_t y, const size_t w, const size_t h )
		: m_file( file )
		, m_x( x )
		, m_y( y )
		, m_w( w )
		, m_h( h ) {}

private:
	const std::string m_file;
	const size_t m_x;
	const size_t m_y;
	const size_t m_w;
	const size_t m_h;
};

}
}
