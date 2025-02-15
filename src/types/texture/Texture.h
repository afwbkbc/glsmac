#pragma once

#include <string>
#include <vector>

#include "types/Serializable.h"

#include "Types.h"

#include "types/Color.h"

namespace common {
class ObjectLink;
}

namespace util {
namespace random {
class Random;
}
class Perlin;
}

namespace types {
namespace texture {

CLASS( Texture, Serializable )
	Texture();
	Texture( const size_t width, const size_t height );
	Texture( const std::string& name, const size_t width, const size_t height );
	virtual ~Texture();

	std::string m_name = "";
	size_t m_width = 0;
	size_t m_height = 0;
	float m_aspect_ratio = 0;
	unsigned char m_bpp = 4; // always RGBA format
	unsigned char* m_bitmap = nullptr;
	size_t m_bitmap_size = 0;

	bool m_is_tiled = false;

	common::ObjectLink* m_graphics_object = nullptr;

	const bool IsEmpty() const;
	void Resize( const size_t width, const size_t height );

	// these methods won't update counter because it would happen too often (and is bad for performance)
	// call Update() manually after you're done
	void SetPixel( const size_t x, const size_t y, const Color::rgba_t& rgba );
	void SetPixel( const size_t x, const size_t y, const Color& color );
	void SetPixelAlpha( const size_t x, const size_t y, const uint8_t alpha );

	const Color::rgba_t GetPixel( const size_t x, const size_t y ) const;

	void Erase( const size_t x1, const size_t y1, const size_t x2, const size_t y2 );

	/**
	 * TODO: refactor this huge parameter list somehow!!!
	 * 
	 * @param source - source Texture object
	 * @param flags - method of how (and when) pixels from source are applied to dest, see add_flag_t
	 * @param x1 - source image part left boundary
	 * @param y1 - source image part top boundary
	 * @param x2 - source image part right boundary
	 * @param y2 - source image part bottom boundary
	 * @param dest_x - (optional) where to start copying to (x coordinate), default 0 (at beginning)
	 * @param dest_y - (optional) where to start copying to (y coordinate), default 0 (at beginning)
	 * @param rotate - (optional) apply as rotated or not, default 0 ( not rotated ). see rotate_t
	 * @param alpha - (optional) parameter for alpha-related flags. !!! For perlin modes it's used as perlin base !!!
	 * @param rng - (optional) random generator for random-related flags
	 * @param perlin - (optional) perlin generator for perlin-related flags
	 */
	void AddFrom( const types::texture::Texture* source, add_flag_t flags, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const size_t dest_x = 0, const size_t dest_y = 0, const rotate_t rotate = 0, const float alpha = 1.0f, util::random::Random* rng = nullptr, util::Perlin* perlin = nullptr );

	void Fill( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const types::Color& color );

	void RepaintFrom( const types::texture::Texture* original, const repaint_rules_t& rules );
	void ColorizeFrom( const types::texture::Texture* original, const types::Color& color, const types::Color& shadow_color );

	void Rotate();
	void FlipV();
	//void FlipH(); // TODO
	void SetAlpha( const float alpha );
	void SetContrast( const float contrast );

	static Texture* FromColor( const Color& color );

	struct updated_area_t {
		size_t left;
		size_t top;
		size_t right;
		size_t bottom;

		const std::string ToString() const {
			return "[ " + std::to_string( left ) + "x" + std::to_string( top ) + " " + std::to_string( right ) + "x" + std::to_string( bottom ) + " ]";
		}
	};
	typedef std::vector< updated_area_t > updated_areas_t;
	updated_areas_t m_updated_areas = {};

	void Update( const updated_area_t updated_area );
	void FullUpdate();
	const size_t UpdatedCount() const;
	const updated_areas_t& GetUpdatedAreas() const;
	void ClearUpdatedAreas();

	// allocates and returns copy of bitmap from specified area
	// don't forget to free() it later
	// supposed to be faster than AddFrom
	unsigned char* CopyBitmap( const size_t x1, const size_t y1, const size_t x2, const size_t y2 ) const;

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

private:
	size_t m_update_counter = 0;
};

}
}
