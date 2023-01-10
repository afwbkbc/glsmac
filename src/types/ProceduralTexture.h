#pragma once

#include "Texture.h"

#include <string>

#include "Color.h"

namespace types {

class ProceduralTexture : public Texture {
public:
	ProceduralTexture( const std::string& name, const size_t width, const size_t height );
	
	void SetPixel( const size_t x, const size_t y, const Color color );
	void Rectangle( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color color );
	
};

}
