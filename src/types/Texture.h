#pragma once

#include <string>

#include "types/Serializable.h"

#include "base/ObjectLink.h"
#include "Color.h"
#include "util/Random.h"
#include "util/Perlin.h"

namespace types {

CLASS( Texture, Serializable )
	Texture( const std::string& name, const size_t width, const size_t height );
	virtual ~Texture();
	
	std::string m_name;
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
	void SetPixelAlpha( const size_t x, const size_t y, const uint8_t alpha );
	
	// bitflags
	typedef uint32_t add_flag_t;
	static constexpr add_flag_t AM_DEFAULT = 0;
	static constexpr add_flag_t AM_MERGE = 1 << 0; // copy only non-transparent pixels
	// round one or several corners
	static constexpr add_flag_t AM_ROUND_LEFT = 1 << 1;
	static constexpr add_flag_t AM_ROUND_TOP = 1 << 2;
	static constexpr add_flag_t AM_ROUND_RIGHT = 1 << 3;
	static constexpr add_flag_t AM_ROUND_BOTTOM = 1 << 4;
	// invert texture while copying, upside down or left-right
	static constexpr add_flag_t AM_INVERT = 1 << 5; // add unneeded pixels instead of needed
	static constexpr add_flag_t AM_MIRROR_X = 1 << 6; // mirrors source by x
	static constexpr add_flag_t AM_MIRROR_Y = 1 << 7; // mirrors source by y
	static constexpr add_flag_t AM_RANDOM_MIRROR_X = 1 << 8; // sometimes mirror by x
	static constexpr add_flag_t AM_RANDOM_MIRROR_Y = 1 << 9; // sometimes mirror by x
	// copy as gradient, alpha will decrease towards center
	static constexpr add_flag_t AM_GRADIENT_LEFT = 1 << 10;
	static constexpr add_flag_t AM_GRADIENT_TOP = 1 << 11;
	static constexpr add_flag_t AM_GRADIENT_RIGHT = 1 << 12;
	static constexpr add_flag_t AM_GRADIENT_BOTTOM = 1 << 13;
	static constexpr add_flag_t AM_GRADIENT_TIGHTER = 1 << 14; // gradient range is twice smaller
	// shift one part before other at random distance
	static constexpr add_flag_t AM_RANDOM_SHIFT_X = 1 << 15;
	static constexpr add_flag_t AM_RANDOM_SHIFT_Y = 1 << 16;
	// generate curved borders by perlin noise
	static constexpr add_flag_t AM_PERLIN_LEFT = 1 << 17;
	static constexpr add_flag_t AM_PERLIN_TOP = 1 << 18;
	static constexpr add_flag_t AM_PERLIN_RIGHT = 1 << 19;
	static constexpr add_flag_t AM_PERLIN_BOTTOM = 1 << 20;
	// use those if perlin pattern shouldn't go till the end (it will gradually reduce towards last 1/3)
	static constexpr add_flag_t AM_PERLIN_CUT_LEFT = 1 << 21;
	static constexpr add_flag_t AM_PERLIN_CUT_TOP = 1 << 22;
	static constexpr add_flag_t AM_PERLIN_CUT_RIGHT = 1 << 23;
	static constexpr add_flag_t AM_PERLIN_CUT_BOTTOM = 1 << 24;
	// adds border to perlin pattern
	static constexpr add_flag_t AM_COASTLINE_BORDER = 1 << 25;
	// randomly stretch texture during copy
	static constexpr add_flag_t AM_RANDOM_STRETCH = 1 << 26; // just stretch up
	static constexpr add_flag_t AM_RANDOM_STRETCH_SHRINK = 1 << 27; // texture can stretch down too
	static constexpr add_flag_t AM_RANDOM_STRETCH_SHIFT = 1 << 28; // also start x and y from random positions
	static constexpr add_flag_t AM_RANDOM_STRETCH_SHUFFLE = 1 << 29; // shuffle more. also implies all other stretch flags
	
	typedef uint8_t rotate_t;
	static constexpr rotate_t ROTATE_0 = 0;
	static constexpr rotate_t ROTATE_90 = 1;
	static constexpr rotate_t ROTATE_180 = 2;
	static constexpr rotate_t ROTATE_270 = 3;
	
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
	void AddFrom( const types::Texture* source, add_flag_t flags, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const size_t dest_x = 0, const size_t dest_y = 0, const rotate_t rotate = 0, const float alpha = 1.0f, util::Random* rng = nullptr, util::Perlin* perlin = nullptr );
	
	void Rotate();
	void FlipV();
	//void FlipH(); // TODO
	void SetAlpha(const float alpha);
	void SetContrast(const float contrast);

	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
};

} /* namespace types */
