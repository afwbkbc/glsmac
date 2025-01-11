#pragma once

#include "types/Color.h"

namespace types {
namespace texture {

class Texture;

class Filter {
public:
	Filter( Texture* const source_texture );
	~Filter();

	struct texture_data_t {
		Texture* texture = nullptr;
		bool is_owned = false;
	};

	const size_t GetWidth() const;
	const size_t GetHeight() const;
	const texture_data_t GetTexture();

	void Tint( const types::Color& color, const float intensity );
	void Crop( const size_t x1, const size_t y1, const size_t x2, const size_t y2 );

private:

	Texture* const m_source_texture;

	// more convenient to process in runtime structure
	size_t m_width = 0;
	size_t m_height = 0;
	types::Color::color_t* m_colormap = nullptr;

	void Prepare();
};

}
}
