#pragma once

#include "base/Base.h"

#include <string>

#include "base/ObjectLink.h"
#include "Color.h"

namespace types {

CLASS( Texture, base::Base )
	Texture( const std::string& name, const size_t width = 0, const size_t height = 0 );
	virtual ~Texture();
	
	const std::string m_name;
	size_t m_width = 0;
	size_t m_height = 0;
	float m_aspect_ratio = 0;
	unsigned char m_bpp = 0;
	unsigned char *m_bitmap = nullptr;
	size_t m_bitmap_size = 0;
	
	bool m_is_tiled = false;

	base::ObjectLink *m_graphics_object = nullptr;
	
	void Resize( const size_t width, const size_t height );
	
	void SetPixel( const size_t x, const size_t y, const Color::rgba_t& rgba );
	void SetPixel( const size_t x, const size_t y, const Color& color );
	void Rectangle( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color color );
	
	void CopyFrom( const types::Texture* source, const size_t x1, const size_t y1, const size_t x2, const size_t y2 );
	void Rotate();
	void FlipV();
	//void FlipH(); // TODO
	void SetAlpha(const float alpha);
	void SetContrast(const float contrast);
};

} /* namespace types */
