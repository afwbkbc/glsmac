#pragma once

#include <string>
#include <vector>

#include "types/Serializable.h"

#include "Types.h"

#include "types/Color.h"

namespace loader::texture {
class SDL2;
}

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

	Texture( const texture_flag_t flags = TF_NONE );
	Texture( const size_t width, const size_t height, const texture_flag_t flags = TF_NONE );
	Texture( const std::string& name, const size_t width, const size_t height, const texture_flag_t flags = TF_NONE );
	virtual ~Texture();

	virtual const bool IsEmpty() const;
	virtual const bool Resize( const size_t width, const size_t height );
	void Clear();
	virtual const size_t GetWidth() const;
	virtual const size_t GetHeight() const;
	virtual unsigned char* const GetBitmap() const;
	virtual const size_t GetBitmapSize() const;
	virtual const std::string& GetFilename() const;

	// these methods won't update counter because it would happen too often (and is bad for performance)
	// call Update() manually after you're done
	virtual void SetPixel( const size_t x, const size_t y, const Color::rgba_t& rgba );
	virtual void SetPixel( const size_t x, const size_t y, const Color& color );
	void SetPixelMaybe( const ssize_t x, const ssize_t y, const Color& color );
	virtual void SetPixelAlpha( const size_t x, const size_t y, const uint8_t alpha );

	virtual const Color::rgba_t GetPixel( const size_t x, const size_t y ) const;

	virtual void Erase( const size_t x1, const size_t y1, const size_t x2, const size_t y2 );

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
	virtual void AddFrom( const types::texture::Texture* source, add_flag_t flags, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const size_t dest_x = 0, const size_t dest_y = 0, const rotate_t rotate = 0, const float alpha = 1.0f, util::random::Random* rng = nullptr, util::Perlin* perlin = nullptr );

	virtual void Fill( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const types::Color& color );

	virtual void RepaintFrom( const types::texture::Texture* original, const repaint_rules_t& rules );
	virtual void ColorizeFrom( const types::texture::Texture* original, const types::Color& color, const types::Color& shadow_color );

	virtual void Rotate();
	virtual void FlipV();
	//void FlipH(); // TODO
	virtual void SetAlpha( const float alpha );
	virtual void SetContrast( const float contrast );

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

	virtual void Update( const updated_area_t updated_area );
	virtual void FullUpdate();
	virtual const size_t UpdatedCount() const;
	virtual const updated_areas_t& GetUpdatedAreas() const;
	virtual void ClearUpdatedAreas();

	// allocates and returns copy of bitmap from specified area
	// don't forget to free() it later
	// supposed to be faster than AddFrom
	virtual unsigned char* CopyBitmap( const size_t x1, const size_t y1, const size_t x2, const size_t y2 ) const;

	virtual const types::Buffer Serialize() const override;
	virtual void Deserialize( types::Buffer buf ) override;

	const bool HasFlag( const texture_flag_t flag ) const;
	const texture_flag_t GetFlags() const;

private:
	size_t m_update_counter = 0;

	const texture_flag_t m_flags = TF_NONE;

	std::string m_filename = "";
	size_t m_width = 0;
	size_t m_height = 0;
	float m_aspect_ratio = 0;
	unsigned char m_bpp = 4; // always RGBA format
	unsigned char* m_bitmap = nullptr;
	size_t m_bitmap_size = 0;

	bool m_is_tiled = false;

	common::ObjectLink* m_graphics_object = nullptr;

private:
	friend class loader::texture::SDL2;
	void SetBitmap( void* const pixels );
};

}
}
