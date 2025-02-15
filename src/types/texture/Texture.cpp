#include "Texture.h"

#include <cstring>
#include <cmath>

#include "common/ObjectLink.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "util/random/Random.h"
#include "util/Perlin.h"

// TODO: refactor, remove map dependency
#include "game/backend/map/Consts.h"

namespace types {
namespace texture {

Texture::Texture() {
	// nothing
}

Texture::Texture( const size_t width, const size_t height ) {
	if ( width > 0 && height > 0 ) {
		Resize( width, height );
	}
}

Texture::Texture( const std::string& name, const size_t width, const size_t height )
	: m_name( name ) {
	if ( width > 0 && height > 0 ) {
		Resize( width, height );
	}
}

Texture::~Texture() {
	if ( g_engine ) { // may be null if shutting down
		g_engine->GetGraphics()->UnloadTexture( this );
	}
	if ( m_bitmap ) {
		free( m_bitmap );
	}
	if ( m_graphics_object ) {
		m_graphics_object->Remove();
	}
}

const bool Texture::IsEmpty() const {
	return m_width == 0 && m_height == 0;
}

void Texture::Resize( const size_t width, const size_t height ) {
	if ( m_width != width || m_height != height ) {

		//Log( "Setting texture size to " + std::to_string( width ) + "x" + std::to_string( height ) );

		m_width = width;
		m_height = height;

		if ( m_bitmap ) {
			free( m_bitmap );
		}

		m_bitmap_size = m_width * m_height * m_bpp;

		if ( m_height > 0 && m_width > 0 ) {

			m_aspect_ratio = m_height / m_width;

			m_bitmap = (unsigned char*)malloc( m_bitmap_size );
			memset( ptr( m_bitmap, 0, m_bitmap_size ), 0, m_bitmap_size );
		}
		else {
			m_bitmap = nullptr;
		}
		FullUpdate();
	}
}

void Texture::SetPixel( const size_t x, const size_t y, const Color::rgba_t& rgba ) {
	memcpy( ptr( m_bitmap, ( y * m_width + x ) * m_bpp, sizeof( rgba ) ), &rgba, sizeof( rgba ) );
}

void Texture::SetPixel( const size_t x, const size_t y, const Color& color ) {
	SetPixel( x, y, color.GetRGBA() );
}

void Texture::SetPixelAlpha( const size_t x, const size_t y, const uint8_t alpha ) {
	memcpy( ptr( m_bitmap, ( y * m_width + x ) * m_bpp + 3, sizeof( alpha ) ), &alpha, sizeof( alpha ) );
}

const Color::rgba_t Texture::GetPixel( const size_t x, const size_t y ) const {
	Color::rgba_t rgba;
	memcpy( &rgba, ptr( m_bitmap, ( y * m_width + x ) * m_bpp, sizeof( rgba ) ), sizeof( rgba ) );
	return rgba;
}

void Texture::Erase( const size_t x1, const size_t y1, const size_t x2, const size_t y2 ) {
	ASSERT( x1 < x2, "x1 must be smaller than x2" );
	ASSERT( y1 < y2, "y1 must be smaller than y2" );
	ASSERT( x1 < m_width, "x1 overflow" );
	ASSERT( y1 < m_height, "y1 overflow" );
	ASSERT( x2 < m_width, "x2 overflow" );
	ASSERT( y2 < m_height, "y2 overflow" );

	for ( auto y = y1 ; y <= y2 ; y++ ) {
		for ( auto x = x1 ; x <= x2 ; x++ ) {
			SetPixel( x, y, 0 );
		}
	}

	Update(
		{
			x1,
			y1,
			x2,
			y2
		}
	);
}

void Texture::Fill( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const types::Color& color ) {
	ASSERT( x2 >= x1, "invalid source x size ( " + std::to_string( x2 ) + " < " + std::to_string( x1 ) + " )" );
	ASSERT( y2 >= y1, "invalid source y size ( " + std::to_string( y2 ) + " < " + std::to_string( y1 ) + " )" );
	ASSERT( x2 < m_width, "x overflow ( " + std::to_string( x2 ) + " >= " + std::to_string( m_width ) + " )" );
	ASSERT( y2 < m_height, "y overflow ( " + std::to_string( y2 ) + " >= " + std::to_string( m_height ) + " )" );

	const uint32_t rgba = color.GetRGBA();
	for ( auto y = y1 ; y <= y2 ; y++ ) {
		for ( auto x = x1 ; x <= x2 ; x++ ) {
			memcpy( ptr( m_bitmap, ( y * m_width + x ) * m_bpp, sizeof( rgba ) ), &rgba, sizeof( rgba ) );
		}
	}
}

void Texture::AddFrom( const types::texture::Texture* source, add_flag_t flags, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const size_t dest_x, const size_t dest_y, const rotate_t rotate, const float alpha, util::random::Random* rng, util::Perlin* perlin ) {
	ASSERT( x2 >= x1, "invalid source x size ( " + std::to_string( x2 ) + " < " + std::to_string( x1 ) + " )" );
	ASSERT( y2 >= y1, "invalid source y size ( " + std::to_string( y2 ) + " < " + std::to_string( y1 ) + " )" );
	ASSERT( dest_x + ( x2 - x1 ) < m_width, "destination x overflow ( " + std::to_string( dest_x + ( x2 - x1 ) ) + " >= " + std::to_string( m_width ) + " )" );
	ASSERT( dest_y + ( y2 - y1 ) < m_height, "destination y overflow (" + std::to_string( dest_y + ( y2 - y1 ) ) + " >= " + std::to_string( m_height ) + " )" );
	ASSERT( alpha >= 0, "invalid alpha value ( " + std::to_string( alpha ) + " < 0 )" );
	ASSERT( alpha <= 1, "invalid alpha value ( " + std::to_string( alpha ) + " > 1 )" );

#define MIX_COLORS( _a, _b, _alpha ) ( \
        (uint8_t)( (float)( (_a) & 0xff ) * (_alpha) + (float)( (_b) & 0xff ) * ( 1.0f - (_alpha) ) ) | \
        (uint8_t)( (float)( (_a) >> 8 & 0xff ) * (_alpha) + (float)( (_b) >> 8 & 0xff ) * ( 1.0f - (_alpha) ) ) << 8 | \
        (uint8_t)( (float)( (_a) >> 16 & 0xff ) * (_alpha) + (float)( (_b) >> 16 & 0xff ) * ( 1.0f - (_alpha) ) ) << 16 | \
        (uint8_t)( (float)( (_a) >> 24 & 0xff ) * (_alpha) + (float)( (_b) >> 24 & 0xff ) * ( 1.0f - (_alpha) ) ) << 24 \
    )

#define COASTLINES_BORDER_RND ( (float)( perlin->Noise( x * 4, y * 4, 1.5f ) + 1.0f ) / 2 * game::backend::map::s_consts.coastlines.border_size )

	// +1 because it's inclusive on both sides
	// TODO: make non-inclusive
	const size_t w = x2 - x1 + 1;
	const size_t h = y2 - y1 + 1;

	const void* from;
	void* to;

	ASSERT( rotate < 4, "invalid rotate value " + std::to_string( rotate ) );
	if ( rotate > 0 ) {
		ASSERT( w == h, "rotating supported only for squares for now" );
	}

	size_t shiftx, shifty; // for random shifts

	// center
	size_t cx = floor( w / 2 );
	size_t cy = floor( h / 2 );

	ssize_t sx, sy; // source
	ssize_t dx, dy; // dest
	float r = 0.0f; // radius for rounded corners
	std::pair< float, float > srx, sry; // stretch ratio ranges
	float ssx_start, ssx, ssy; // stretched source

	// for perlin borders
	size_t perlin_maxx[h];
	memset( &perlin_maxx, 0, sizeof( perlin_maxx ) );
	size_t perlin_maxy[w];
	memset( &perlin_maxy, 0, sizeof( perlin_maxy ) );

	if (
		( flags & types::texture::AM_ROUND_LEFT ) ||
			( flags & types::texture::AM_ROUND_TOP ) ||
			( flags & types::texture::AM_ROUND_RIGHT ) ||
			( flags & types::texture::AM_ROUND_BOTTOM ) ||
			( flags & types::texture::AM_GRADIENT_LEFT ) ||
			( flags & types::texture::AM_GRADIENT_TOP ) ||
			( flags & types::texture::AM_GRADIENT_RIGHT ) ||
			( flags & types::texture::AM_GRADIENT_BOTTOM )
		) {

		r = sqrt( pow( (float)cx, 2 ) + pow( (float)cy, 2 ) );
		if (
			( flags & types::texture::AM_ROUND_LEFT ) ||
				( flags & types::texture::AM_ROUND_TOP ) ||
				( flags & types::texture::AM_ROUND_RIGHT ) ||
				( flags & types::texture::AM_ROUND_BOTTOM )
			) {
			ASSERT( flags & types::texture::AM_MERGE, "rounded corners supported only for merge flags" );
			r *= 0.7f;
			ASSERT( r >= 0, "sqrt returned negative result: " + std::to_string( r ) );
		}
	}

	if ( flags & types::texture::AM_RANDOM_MIRROR_X ) {
		ASSERT( rng, "no rng provided for random mirror" );
		if ( rng->IsLucky( 2 ) ) {
			flags ^= types::texture::AM_MIRROR_X;
		}
	}

	if ( flags & types::texture::AM_RANDOM_MIRROR_Y ) {
		ASSERT( rng, "no rng provided for random mirror" );
		if ( rng->IsLucky( 2 ) ) {
			flags ^= types::texture::AM_MIRROR_Y;
		}
	}

	if ( flags & types::texture::AM_RANDOM_SHIFT_X ) {
		ASSERT( rng, "no rng provided for random shift" );
		shiftx = rng->GetUInt( 0, w - 1 );
	}
	else {
		shiftx = 0;
	}

	if ( flags & types::texture::AM_RANDOM_SHIFT_Y ) {
		ASSERT( rng, "no rng provided for random shift" );
		shifty = rng->GetUInt( 0, h - 1 );
	}
	else {
		shifty = 0;
	}

	float pixel_alpha;

	bool is_pixel_needed;
	Color::rgba_t mix_color;

	if ( flags & ( types::texture::AM_PERLIN_LEFT | types::texture::AM_PERLIN_TOP | types::texture::AM_PERLIN_RIGHT | types::texture::AM_PERLIN_BOTTOM ) ) {

		ASSERT( rng, "no rng provided for perlin edge" );
		ASSERT( perlin, "no perlin provided for perlin edge" );

		// perlin base // TODO: pattern continuation between tiles

		// if we go outside 0.0 - 1.0 range then edges between tiles won't align properly
		// if we stay inside 0.0 - 1.0 range then pattern is too repeative
		const float pb = alpha * 1000000; // TODO: refactor the whole thing!

		// consts
		// TODO: pass from parameters somehow (need to refactor)
		//const float pb = ( flags & types::texture::AM_COASTLINE_BORDER ) ? 0.1f : 0;
		const float pr = game::backend::map::s_consts.coastlines.perlin.range;// + ( ( flags & types::texture::AM_PERLIN_WIDER ) ? 0.1f : 0 );
		const float pf = game::backend::map::s_consts.coastlines.perlin.frequency;
		const uint8_t pp = game::backend::map::s_consts.coastlines.perlin.passes;
		const float pc = game::backend::map::s_consts.coastlines.perlin.cut;

		// perlin range (for cutting)
		std::pair< size_t, size_t > prx = {
			0,
			w
		};
		std::pair< size_t, size_t > pry = {
			0,
			h
		};

		// temp vars
		size_t key;

		if ( flags & types::texture::AM_PERLIN_CUT_LEFT ) {
			prx.first += round( (float)w * pc );
		}
		if ( flags & types::texture::AM_PERLIN_CUT_TOP ) {
			pry.first += round( (float)h * pc );
		}
		if ( flags & types::texture::AM_PERLIN_CUT_RIGHT ) {
			prx.second -= round( (float)w * pc );
		}
		if ( flags & types::texture::AM_PERLIN_CUT_BOTTOM ) {
			pry.second -= round( (float)h * pc );
		}

		if ( flags & ( types::texture::AM_PERLIN_LEFT | types::texture::AM_PERLIN_RIGHT ) ) {
			for ( auto y = 0 ; y < h ; y++ ) {
				key = ( flags & types::texture::AM_PERLIN_LEFT )
					? y
					: h - y - 1;
				if ( key >= pry.first && key <= pry.second ) {
					perlin_maxx[ key ] = std::max( 1.0f, ( perlin->Noise( pb, (float)y * pf, 0.5f, pp ) + 1.0f ) / 2 * (float)h * pr );
				}
				else {
					perlin_maxx[ key ] = 0;
				}
				//Log( "Perlin maxx[" + std::to_string( y ) + "] = " + std::to_string( perlin_maxx[ y ] ) );
			}
		}
		if ( flags & ( types::texture::AM_PERLIN_TOP | types::texture::AM_PERLIN_BOTTOM ) ) {
			for ( auto x = 0 ; x < w ; x++ ) {
				key = ( flags & types::texture::AM_PERLIN_TOP )
					? x
					: w - x - 1;
				if ( key >= prx.first && key <= prx.second ) {
					perlin_maxy[ key ] = std::max( 1.0f, ( perlin->Noise( (float)x * pf, pb, 0.5f, pp ) + 1.0f ) / 2 * (float)w * pr );
				}
				else {
					perlin_maxy[ key ] = 0;
				}
				//Log( "Perlin maxy[" + std::to_string( x ) + "] = " + std::to_string( perlin_maxy[ x ] ) );
			}
		}
	}

	if ( flags & types::texture::AM_RANDOM_STRETCH_SHUFFLE ) {
		flags |= types::texture::AM_RANDOM_STRETCH | types::texture::AM_RANDOM_STRETCH_SHRINK | types::texture::AM_RANDOM_STRETCH_SHIFT;
	}

	if ( flags & types::texture::AM_RANDOM_STRETCH ) {
		ASSERT( rng, "no rng provided for random mirror" );
		// randomize random ratio ranges (per-tile)
		const float rmin = game::backend::map::s_consts.tile.random.texture_edge_stretch_min;
		const float rmax = game::backend::map::s_consts.tile.random.texture_edge_stretch_max;
		srx = {
			rng->GetFloat( rmin, rmax ),
			( flags & types::texture::AM_RANDOM_STRETCH_SHRINK )
				? rng->GetFloat( rmin, rmax )
				: 0.0f
		};
		sry = {
			rng->GetFloat( rmin, rmax ),
			( flags & types::texture::AM_RANDOM_STRETCH_SHRINK )
				? rng->GetFloat( rmin, rmax )
				: 0.0f
		};
		//Log( "srx=" + std::to_string( srx ) + " sry=" + std::to_string( sry ) );
		// they will (mostly) follow x and y, but with added random 'speed'
		ssx_start = ( srx.first + srx.second ) / 2 * w / 2;
		ssy = ( sry.first + sry.second ) / 2 * h / 2;
		if ( flags & types::texture::AM_RANDOM_STRETCH_SHIFT ) {
			ssx_start += rng->GetUInt( 0, w - 1 ) * ( 1.0f + srx.second );
			ssy += rng->GetUInt( 0, h - 1 ) * ( 1.0f + sry.second );
		}
		ssx = ssx_start + rng->GetFloat( -srx.first, srx.second );
		ssy += rng->GetFloat( -sry.first, sry.second );
	}

#ifdef DEBUG
	// extra checks to make sure every destination pixel was processed (and only once)
	bool px_processed[w][h];
	memset( px_processed, false, sizeof( px_processed ) );
#endif

	// spammy
	//Log( "Texture processing begin" );

	for ( size_t y = 0 ; y < h ; y++ ) {
		for ( size_t x = 0 ; x < w ; x++ ) {

			mix_color = 0;

			switch ( rotate ) {
				case ROTATE_0: {
					dx = x;
					dy = y;
					break;
				}
				case ROTATE_90: {
					dx = h - y - 1;
					dy = x;
					break;
				}
				case ROTATE_180: {
					dx = w - x - 1;
					dy = h - y - 1;
					break;
				}
				case ROTATE_270: {
					dx = y;
					dy = w - x - 1;
					break;
				}
				default: {
					THROW( "invalid rotate value " + std::to_string( rotate ) );
				}
			}

			if ( flags & types::texture::AM_RANDOM_SHIFT_X ) {
				if ( dx >= shiftx ) {
					dx -= shiftx + 1;
				}
				else {
					dx = w - dx - 1;
				}
			}
			if ( flags & types::texture::AM_RANDOM_SHIFT_Y ) {
				if ( dy >= shifty ) {
					dy -= shifty;
				}
				else {
					dy = h - dy - 1;
				}
			}

			is_pixel_needed = true;
			pixel_alpha = alpha;

			if (
				( ( flags & types::texture::AM_ROUND_LEFT ) && ( dx <= cx ) && ( dy >= cy ) ) ||
					( ( flags & types::texture::AM_ROUND_TOP ) && ( dx <= cx ) && ( dy <= cy ) ) ||
					( ( flags & types::texture::AM_ROUND_RIGHT ) && ( dx >= cx ) && ( dy <= cy ) ) ||
					( ( flags & types::texture::AM_ROUND_BOTTOM ) && ( dx >= cx ) && ( dy >= cy ) )
				) {
				float d = sqrt( pow( (float)dx - cx, 2 ) + pow( (float)dy - cy, 2 ) );
				if ( flags & types::texture::AM_COASTLINE_BORDER ) {
					ASSERT( perlin, "perlin for coastline border not set" );
					d = std::min( d, d - (float)sqrt( pow( COASTLINES_BORDER_RND, 2 ) * 2 ) + game::backend::map::s_consts.coastlines.border_size / 2 );
				}
				if ( d > r ) {
					is_pixel_needed = false;
				}
				if ( flags & types::texture::AM_COASTLINE_BORDER ) {
					if ( d >= r - game::backend::map::s_consts.coastlines.perlin.round_range ) {
						// TODO: fix for types::texture::AM_INVERT
						mix_color = game::backend::map::s_consts.coastlines.border_color.GetRGBA();
					}
				}
			}

			if ( flags & ( types::texture::AM_PERLIN_LEFT | types::texture::AM_PERLIN_TOP | types::texture::AM_PERLIN_RIGHT | types::texture::AM_PERLIN_BOTTOM ) ) {

				bool perlin_need_pixel = false; // combine all enabled perlin edges, at least one positive is needed to keep pixel
				bool perlin_need_border = false;

				if ( flags & types::texture::AM_PERLIN_LEFT ) {
					if ( !perlin_need_pixel && x < perlin_maxx[ y ] ) {
						perlin_need_pixel = true;
					}
					if ( !perlin_need_pixel && !perlin_need_border && ( flags & types::texture::AM_COASTLINE_BORDER ) ) {
						if (
							( perlin_maxx[ y ] && x < perlin_maxx[ y ] + COASTLINES_BORDER_RND ) ||
								( perlin_maxx[ y - 1 ] && y > 0 && x <= perlin_maxx[ y - 1 ] ) ||
								( perlin_maxx[ y + 1 ] && y < h - 1 && x <= perlin_maxx[ y + 1 ] )
							) {
							perlin_need_border = true;
						}
					}
				}
				if ( flags & types::texture::AM_PERLIN_TOP ) {
					if ( !perlin_need_pixel && y < perlin_maxy[ x ] ) {
						perlin_need_pixel = true;
					}
					if ( !perlin_need_pixel && !perlin_need_border && ( flags & types::texture::AM_COASTLINE_BORDER ) ) {
						if (
							( perlin_maxy[ x ] && y < perlin_maxy[ x ] + COASTLINES_BORDER_RND ) ||
								( perlin_maxy[ x - 1 ] && x > 0 && y <= perlin_maxy[ x - 1 ] ) ||
								( perlin_maxy[ x + 1 ] && x < w - 1 && y <= perlin_maxy[ x + 1 ] )
							) {
							perlin_need_border = true;
						}
					}
				}
				if ( flags & types::texture::AM_PERLIN_RIGHT ) {
					if ( !perlin_need_pixel && ( w - x ) < perlin_maxx[ y ] ) {
						perlin_need_pixel = true;
					}
					if ( !perlin_need_pixel && !perlin_need_border && ( flags & types::texture::AM_COASTLINE_BORDER ) ) {
						if (
							( perlin_maxx[ y ] && ( w - x ) < perlin_maxx[ y ] + COASTLINES_BORDER_RND ) ||
								( y > 0 && perlin_maxx[ y - 1 ] && ( w - x ) <= perlin_maxx[ y - 1 ] ) ||
								( perlin_maxx[ y + 1 ] && y < h - 1 && ( w - x ) <= perlin_maxx[ y + 1 ] )
							) {
							perlin_need_border = true;
						}
					}
				}
				if ( flags & types::texture::AM_PERLIN_BOTTOM ) {
					if ( !perlin_need_pixel && ( h - y ) < perlin_maxy[ x ] ) {
						perlin_need_pixel = true;
					}
					if ( !perlin_need_pixel && !perlin_need_border && ( flags & types::texture::AM_COASTLINE_BORDER ) ) {
						if (
							( perlin_maxy[ x ] && ( h - y ) < perlin_maxy[ x ] + COASTLINES_BORDER_RND ) ||
								( x > 0 && perlin_maxy[ x - 1 ] && ( h - y ) <= perlin_maxy[ x - 1 ] ) ||
								( perlin_maxy[ x + 1 ] && x < w - 1 && ( h - y ) <= perlin_maxy[ x + 1 ] )
							) {
							perlin_need_border = true;
						}
					}
				}

				if ( !perlin_need_pixel ) {
					is_pixel_needed = false;
				}

				if ( perlin_need_border ) {
					is_pixel_needed = true;
					mix_color = game::backend::map::s_consts.coastlines.border_color.GetRGBA();
				}

			}

			ASSERT( dx >= 0, "dx < 0" );
			ASSERT( dx <= w, "dx > w" );
			ASSERT( dy >= 0, "dy < 0" );
			ASSERT( dy <= h, "dy > h" );
			to = ptr( m_bitmap, ( ( dy + dest_y ) * m_width + ( dx + dest_x ) ) * m_bpp, m_bpp );

			if ( is_pixel_needed && ( flags & types::texture::AM_KEEP_TRANSPARENCY ) ) {
				// don't overwrite transparent pixels
				if ( !*( (uint8_t*)( to ) + 3 ) ) {
					is_pixel_needed = false;
				}
			}

			if ( flags & types::texture::AM_INVERT ) {
				is_pixel_needed = !is_pixel_needed;
			}

			if ( is_pixel_needed ) {
				if ( flags & types::texture::AM_RANDOM_STRETCH ) {
					// round and wrap if needed
					sx = floor( ssx );
					while ( sx < 0 ) {
						sx += w;
					}
					while ( sx >= w ) {
						sx -= w;
					}
					sy = floor( ssy );
					while ( sy < 0 ) {
						sy += h;
					}
					while ( sy > h - 1 ) {
						sy -= h - 1;
					}
				}
				else {
					sx = x;
					sy = y;
				}

				if ( flags & types::texture::AM_MIRROR_X ) {
					sx = x2 - sx;
				}
				else {
					sx = sx + x1;
				}

				if ( flags & types::texture::AM_MIRROR_Y ) {
					sy = y2 - sy;
				}
				else {
					sy = sy + y1;
				}

				// VERY spammy
				/*if ( flags & types::texture::AM_RANDOM_STRETCH ) {
					Log( "SSX=" + std::to_string( ssx ) + " SSY=" + std::to_string( ssy ) + " SX=" + std::to_string( sx ) + " SY=" + std::to_string( sy ) + " DX=" + std::to_string( dx ) + " DY=" + std::to_string( dy ) );
				}*/

#ifdef DEBUG
				ASSERT( !px_processed[ dx ][ dy ], "pixel at " + std::to_string( dx ) + "x" + std::to_string( dy ) + " was already processed" );
				px_processed[ dx ][ dy ] = true;
#endif
				ASSERT( sx >= x1, "sx < x1" );
				ASSERT( sx <= x2, "sx > x2" );
				ASSERT( sy >= y1, "sy < y1" );
				ASSERT( sy <= y2, "sy > y2" );
				from = ptr( source->m_bitmap, ( ( sy ) * source->m_width + ( sx ) ) * m_bpp, m_bpp );

#ifdef DEBUG
				{
					uint32_t pixel_color;
					memcpy( &pixel_color, from, m_bpp );

					// this color is used in texture.pcx (and maybe others) between texture blocks
					// if it got read - then it means source coordinates got wrong
					// some day it may start giving false positives if valid texture happens to have pixel of same color, but it's unlikely
					ASSERT(
						pixel_color != Color::RGBA( 0, 255, 252, 255 ),
						"source texture overflow (tried to read pixel " + std::to_string( sx ) + "x" + std::to_string( sy ) + ")" +
							" x1=" + std::to_string( x1 ) + " y1=" + std::to_string( y1 ) + " x2=" + std::to_string( x2 ) + " y2=" + std::to_string( y2 )
					);
				}
#endif

				if ( ( !( flags & types::texture::AM_MERGE ) ) || ( *(uint32_t*)from & 0xff000000 ) ) {

					if (
						( flags & types::texture::AM_GRADIENT_LEFT ) ||
							( flags & types::texture::AM_GRADIENT_TOP ) ||
							( flags & types::texture::AM_GRADIENT_RIGHT ) ||
							( flags & types::texture::AM_GRADIENT_BOTTOM )
						) {

						uint32_t pixel_color;
						memcpy( &pixel_color, from, m_bpp );
						if ( mix_color ) {
							pixel_color = MIX_COLORS( mix_color, pixel_color, game::backend::map::s_consts.coastlines.border_alpha );
						}

						uint32_t dst_pixel_color;
						memcpy( &dst_pixel_color, to, m_bpp );

						size_t range = ( flags & types::texture::AM_GRADIENT_TIGHTER )
							? 1
							: 2;

						float p = 0.0f;

						if ( flags & types::texture::AM_GRADIENT_LEFT ) {
							if ( flags & types::texture::AM_GRADIENT_TOP ) {
								if ( ( dx + dy ) < ( cx + cy ) ) {
									p = ( (float)( ( cx + cy ) - ( dx + dy ) ) / ( w + h ) * range );
								}
							}
							else if ( flags & types::texture::AM_GRADIENT_BOTTOM ) {
								if ( ( dx + cy ) < ( cx + dy ) ) {
									p = ( (float)( ( cx + dy ) - ( dx + cy ) ) / ( w + h ) * range );
								}
							}
							else {
								if ( dx < cx ) {
									p = (float)( cx - dx ) / w * range;
								}
							}
						}
						else if ( flags & types::texture::AM_GRADIENT_RIGHT ) {
							if ( flags & types::texture::AM_GRADIENT_TOP ) {
								if ( ( cx + dy ) < ( dx + cy ) ) {
									p = ( (float)( ( dx + cy ) - ( cx + dy ) ) / ( w + h ) * range );
								}
							}
							else if ( flags & types::texture::AM_GRADIENT_BOTTOM ) {
								if ( ( cx + cy ) < ( dx + dy ) ) {
									p = ( (float)( ( dx + dy ) - ( cx + cy ) ) / ( w + h ) * range );
								}
							}
							else {
								if ( cx < dx ) {
									p = (float)( dx - cx ) / w * range;
								}
							}
						}
						else if ( flags & types::texture::AM_GRADIENT_TOP ) {
							if ( dy < cy ) {
								p = (float)( cy - dy ) / h * range;
							}
						}
						else if ( flags & types::texture::AM_GRADIENT_BOTTOM ) {
							if ( cy < dy ) {
								p = (float)( dy - cy ) / h * range;
							}
						}

						if ( pixel_alpha < 1.0f ) {
							p *= pixel_alpha;
						}

						pixel_color = MIX_COLORS( pixel_color, dst_pixel_color, p );
						memcpy( to, &pixel_color, m_bpp );
					}
					else {
						if ( mix_color ) {
							uint32_t pixel_color;
							memcpy( &pixel_color, from, m_bpp );
							pixel_color = MIX_COLORS( mix_color, pixel_color, game::backend::map::s_consts.coastlines.border_alpha );
							memcpy( to, &pixel_color, m_bpp );
						}
						else {
							memcpy( to, from, m_bpp );
						}
						if ( pixel_alpha < 1.0f ) {
							if ( flags & types::texture::AM_MERGE ) {

								// TODO: refactor

								uint32_t pixel_color;
								memcpy( &pixel_color, from, m_bpp );
								if ( mix_color ) {
									pixel_color = MIX_COLORS( mix_color, pixel_color, game::backend::map::s_consts.coastlines.border_alpha );
								}
								uint32_t dst_pixel_color;
								memcpy( &dst_pixel_color, to, m_bpp );

								pixel_color = MIX_COLORS( pixel_color, dst_pixel_color, pixel_alpha );

								memcpy( to, &pixel_color, m_bpp );
							}
							else {
								*( (uint8_t*)( to ) + 3 ) = (uint8_t)floor( pixel_alpha * 0xff );
							}
						}
					}
				}
			}
#ifdef DEBUG
			else {
				ASSERT( !px_processed[ dx ][ dy ], "pixel at " + std::to_string( dx ) + "x" + std::to_string( dy ) + " was already processed" );
				px_processed[ dx ][ dy ] = true;
			}
#endif

			if ( flags & types::texture::AM_RANDOM_STRETCH ) {
				ssx += rng->GetFloat( 1.0f - srx.first, 1.0f + srx.second );
			}
		}
		if ( flags & types::texture::AM_RANDOM_STRETCH ) {
			ssx = ssx_start + rng->GetFloat( -srx.first, srx.second );
			ssy += rng->GetFloat( 1.0f - sry.first, 1.0f + sry.second );
		}
	}

#ifdef DEBUG
	// make sure every pixel was processed
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = 0 ; x < w ; x++ ) {
			ASSERT( px_processed[ x ][ y ], "pixel at " + std::to_string( x ) + "x" + std::to_string( y ) + " was not processed (flags: " + std::to_string( flags ) + ")" );
		}
	}
#endif

	// spammy
	//Log( "Texture processing end" );

#undef MIX_COLORS
#undef COASTLINES_BORDER_RND

	Update(
		{
			dest_x,
			dest_y,
			dest_x + w - 1,
			dest_y + h - 1
		}
	);
}

void Texture::RepaintFrom( const types::texture::Texture* original, const repaint_rules_t& rules ) {
	ASSERT( m_width == original->m_width, "repaint width mismatch" );
	ASSERT( m_height == original->m_height, "repaint width mismatch" );
	ASSERT( m_bpp == original->m_bpp, "repaint bpp mismatch" );
	ASSERT( m_bitmap, "bitmap not set" );
	ASSERT( original->m_bitmap, "original bitmap not set" );

	uint32_t rgba;
	repaint_rules_t::const_iterator rule_it;
	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = 0 ; x < m_width ; x++ ) {
			const auto idx = ( y * m_width + x ) * m_bpp;
			memcpy( &rgba, ptr( original->m_bitmap, idx, m_bpp ), m_bpp );
			if ( ( rule_it = rules.find( rgba ) ) != rules.end() ) {
				rgba = rule_it->second;
			}
			memcpy( ptr( m_bitmap, idx, m_bpp ), &rgba, m_bpp );
		}
	}

}

void Texture::ColorizeFrom( const types::texture::Texture* original, const types::Color& color, const types::Color& shadow_color ) {
	ASSERT( m_width == original->m_width, "repaint width mismatch" );
	ASSERT( m_height == original->m_height, "repaint width mismatch" );
	ASSERT( m_bpp == original->m_bpp, "repaint bpp mismatch" );
	ASSERT( m_bitmap, "bitmap not set" );
	ASSERT( original->m_bitmap, "original bitmap not set" );

	uint32_t rgba;
	repaint_rules_t::const_iterator rule_it;
	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = 0 ; x < m_width ; x++ ) {
			const auto idx = ( y * m_width + x ) * m_bpp;
			memcpy( &rgba, ptr( original->m_bitmap, idx, m_bpp ), m_bpp );
			if ( rgba ) {
				const auto c = types::Color::FromRGBA( rgba );
				if ( !c.value.red && !c.value.green && !c.value.blue ) {
					rgba = ( c * shadow_color ).GetRGBA();
				}
				else {
					rgba = ( c * color ).GetRGBA();
				}
				memcpy( ptr( m_bitmap, idx, m_bpp ), &rgba, m_bpp );
			}
		}
	}
}

void Texture::Rotate() {
	unsigned char* new_bitmap = (unsigned char*)malloc( m_bitmap_size );

	const size_t tmp = m_width;
	m_width = m_height;
	m_height = tmp;

	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = 0 ; x < m_width ; x++ ) {
			memcpy(
				ptr( new_bitmap, ( y * m_width + x ) * m_bpp, m_bpp ),
				ptr( m_bitmap, ( x * m_height + y ) * m_bpp, m_bpp ),
				m_bpp
			);
		}
	}

	free( m_bitmap );
	m_bitmap = new_bitmap;

	FullUpdate();
}

void Texture::FlipV() {
	unsigned char* new_bitmap = (unsigned char*)malloc( m_bitmap_size );

	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = 0 ; x < m_width ; x++ ) {
			memcpy(
				ptr( new_bitmap, ( y * m_width + x ) * m_bpp, m_bpp ),
				ptr( m_bitmap, ( ( m_height - y - 1 ) * m_width + x ) * m_bpp, m_bpp ),
				m_bpp
			);
		}
	}

	free( m_bitmap );
	m_bitmap = new_bitmap;

	FullUpdate();
}

void Texture::SetAlpha( const float alpha ) {
	const uint8_t alpha_byte = alpha * 255;
	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = 0 ; x < m_width ; x++ ) {
			*ptr( m_bitmap, ( y * m_width + x ) * m_bpp + 3, sizeof( alpha_byte ) ) = alpha_byte;
		}
	}

	FullUpdate();
}

void Texture::SetContrast( const float contrast ) {
	size_t i;
	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = 0 ; x < m_width ; x++ ) {
			for ( size_t b = 0 ; b < 3 ; b++ ) {
				i = ( y * m_width + x ) * m_bpp + b;
				*ptr( m_bitmap, i, sizeof( unsigned char ) ) = (unsigned char)floor( std::fmin( 255, (float)m_bitmap[ i ] * contrast ) );
			}
		}
	}

	FullUpdate();
}

Texture* Texture::FromColor( const Color& color ) {
	NEWV( texture, Texture, "FromColor", 1, 1 );
	texture->SetPixel( 0, 0, color );
	return texture;
}

void Texture::Update( const updated_area_t updated_area ) {
	//Log( "Need texture update [ "+ std::to_string( updated_area.left ) + " " + std::to_string( updated_area.top ) + " " + std::to_string( updated_area.right ) + " " + std::to_string( updated_area.bottom ) + " ]" );
	m_updated_areas.push_back( updated_area );
	m_update_counter++;
}

void Texture::FullUpdate() {
	ClearUpdatedAreas();
	Update(
		{
			0,
			0,
			m_width,
			m_height
		}
	);
}

const size_t Texture::UpdatedCount() const {
	return m_update_counter;
}

const Texture::updated_areas_t& Texture::GetUpdatedAreas() const {
	return m_updated_areas;
}

void Texture::ClearUpdatedAreas() {
	m_updated_areas.clear();
}

unsigned char* Texture::CopyBitmap( const size_t x1, const size_t y1, const size_t x2, const size_t y2 ) const {

	ASSERT( x1 < x2, "x1 must be smaller than x2" );
	ASSERT( y1 < y2, "y1 must be smaller than y2" );
	ASSERT( x1 <= m_width, "x1 overflow" );
	ASSERT( y1 <= m_height, "y1 overflow" );
	ASSERT( x2 <= m_width, "x2 overflow" );
	ASSERT( y2 <= m_height, "y2 overflow" );

	const size_t w = x2 - x1;
	const size_t h = y2 - y1;
	const uint8_t bpp = 4;

	const size_t wbpp = w * bpp;

	unsigned char* bitmap = (unsigned char*)malloc( w * h * bpp );
	for ( size_t y = 0 ; y < h ; y++ ) {
		memcpy(
			ptr( bitmap, y * wbpp, wbpp ),
			ptr( m_bitmap, ( ( y1 + y ) * m_width + x1 ) * bpp, wbpp ),
			wbpp
		);
	}

	return bitmap;
}

const types::Buffer Texture::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_name );
	buf.WriteInt( m_width );
	buf.WriteInt( m_height );
	buf.WriteFloat( m_aspect_ratio );
	buf.WriteInt( m_bpp );

	buf.WriteInt( m_bitmap_size );
	buf.WriteData( m_bitmap, m_bitmap_size );

	buf.WriteBool( m_is_tiled );

	return buf;
}

void Texture::Unserialize( types::Buffer buf ) {

	m_name = buf.ReadString();
	size_t width = buf.ReadInt();
	if ( width != m_width ) {
		THROW( "texture read width mismatch ( " + std::to_string( width ) + " != " + std::to_string( m_width ) + " )" );
	}
	size_t height = buf.ReadInt();
	if ( height != m_height ) {
		THROW( "texture read height mismatch ( " + std::to_string( height ) + " != " + std::to_string( m_height ) + " )" );
	}

	m_aspect_ratio = buf.ReadFloat();

	m_bpp = buf.ReadInt();
	ASSERT( m_bpp == 4, "invalid bpp" );

	m_bitmap_size = buf.ReadInt();

	if ( m_bitmap ) {
		free( m_bitmap );
	}
	m_bitmap = (unsigned char*)buf.ReadData( m_bitmap_size );

	m_is_tiled = buf.ReadBool();

	FullUpdate();
}

}
}
