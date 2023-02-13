#include <cstring>
#include <cmath>
#include <iostream>

#include "Texture.h"
#include "engine/Engine.h"

namespace types {

Texture::Texture( const std::string& name, const size_t width, const size_t height )
	: m_name( name )
{
	m_bpp = 4; // always RGBA format
	
	if (width > 0 && height > 0) {
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
	if ( m_graphics_object )
		m_graphics_object->Remove();
}

void Texture::Resize( const size_t width, const size_t height ) {
	
	if (m_width != width || m_height != height) {
		
		m_width = width;
		m_height = height;
	
		m_aspect_ratio = m_height / m_width;

		if ( m_bitmap ) {
			free( m_bitmap );
		}
		m_bitmap_size = m_width * m_height * m_bpp;
		m_bitmap = (unsigned char*) malloc( m_bitmap_size );
		memset( ptr( m_bitmap, 0, m_bitmap_size ), 0, m_bitmap_size );
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

void Texture::AddFrom( const types::Texture* source, const add_mode_t mode, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const size_t dest_x, const size_t dest_y, const rotate_t rotate, const float alpha ) {
	ASSERT( dest_x + ( x2 - x1 ) < m_width, "destination x overflow ( " + std::to_string( dest_x + ( x2 - x1 ) ) + " >= " + std::to_string( m_width ) + " )" );
	ASSERT( dest_y + ( y2 - y1 ) < m_height, "destination y overflow (" + std::to_string( dest_y + ( y2 - y1 ) ) + " >= " + std::to_string( m_height ) + " )" );
	ASSERT( x2 >= x1, "invalid source x size ( " + std::to_string( x2 ) + " < " + std::to_string( x1 ) + " )" );
	ASSERT( y2 >= y1, "invalid source y size ( " + std::to_string( y2 ) + " < " + std::to_string( y1 ) + " )" );
	ASSERT( alpha >= 0, "invalid alpha value ( " + std::to_string( alpha ) + " < 0 )" );
	ASSERT( alpha <= 1, "invalid alpha value ( " + std::to_string( alpha ) + " > 1 )" );
	
	// +1 because it's inclusive on both sides
	// TODO: make non-inclusive
	const size_t w = x2 - x1 + 1;
	const size_t h = y2 - y1 + 1;
	
	const void *from;
	void *to;
	
	ASSERT( rotate < 4, "invalid rotate value " + std::to_string( rotate ) );
	if ( rotate > 0 ) {
		ASSERT( w == h, "rotating supported only for squares for now" );
	}
	
	size_t cx, cy; // center
	size_t sx, sy; // source
	size_t dx, dy; // dest
	float r;
	
	if (
		( mode & AM_ROUND_LEFT ) ||
		( mode & AM_ROUND_TOP ) ||
		( mode & AM_ROUND_RIGHT ) ||
		( mode & AM_ROUND_BOTTOM ) ||
		( mode & AM_GRADIENT_LEFT ) ||
		( mode & AM_GRADIENT_TOP ) ||
		( mode & AM_GRADIENT_RIGHT ) ||
		( mode & AM_GRADIENT_BOTTOM )
	) {
		cx = floor( w / 2 );
		cy = floor( h / 2 );
		r = sqrt( pow( (float)cx, 2 ) + pow( (float)cy, 2 ) );
		if (
			( mode & AM_ROUND_LEFT ) ||
			( mode & AM_ROUND_TOP ) ||
			( mode & AM_ROUND_RIGHT ) ||
			( mode & AM_ROUND_BOTTOM )
		) {
			ASSERT( mode & AM_MERGE, "rounded corners supported only for merge mode" );
			r *= 0.7f;
			ASSERT( r >= 0, "sqrt returned negative result: " + std::to_string( r ) );
		}
	}
	
	float pixel_alpha;
	
	bool is_pixel_needed;
	
	for (size_t y = 0 ; y < h ; y++) {
		for (size_t x = 0 ; x < w; x++) {
			
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
					ASSERT( false, "invalid rotate value " + std::to_string( rotate ) );
				}
			}
			
			is_pixel_needed = true;
			pixel_alpha = alpha;
			
			if (
				( ( mode & AM_ROUND_LEFT ) && ( dx <= cx ) && ( dy >= cy ) ) ||
				( ( mode & AM_ROUND_TOP ) && ( dx <= cx ) && ( dy <= cy ) ) ||
				( ( mode & AM_ROUND_RIGHT ) && ( dx >= cx ) && ( dy <= cy ) ) ||
				( ( mode & AM_ROUND_BOTTOM ) && ( dx >= cx ) && ( dy >= cy ) )
			) {
				float d = sqrt( pow( (float) dx - cx, 2 ) + pow( (float) dy - cy, 2 ) );
				if ( d > r ) {
					is_pixel_needed = false;
				}
				else if ( d == r ) {
					pixel_alpha = 0.5f; // some smoothing
				}
			}
			
			if ( mode & AM_INVERT ) {
				is_pixel_needed = !is_pixel_needed;
			}
			
			if ( is_pixel_needed ) {
				if ( mode & AM_MIRROR_X ) {
					sx = x2 - x;
				}
				else {
					sx = x + x1;
				}
				if ( mode & AM_MIRROR_Y ) {
					sy = y2 - y;
				}
				else {
					sy = y + y1;
				}
				from = ptr( source->m_bitmap, ( ( sy ) * source->m_width + ( sx ) ) * m_bpp, m_bpp );
				to = ptr( m_bitmap, ( ( dy + dest_y ) * m_width + ( dx + dest_x ) ) * m_bpp, m_bpp );

				if ( ( !( mode & AM_MERGE ) ) || ( *(uint32_t*)from & 0x000000ff ) ) {
					
					if (
						( mode & AM_GRADIENT_LEFT ) ||
						( mode & AM_GRADIENT_TOP ) ||
						( mode & AM_GRADIENT_RIGHT ) ||
						( mode & AM_GRADIENT_BOTTOM )
					) {
					
						uint32_t pixel_color;
						memcpy( &pixel_color, from, m_bpp );

						uint32_t dst_pixel_color;
						memcpy( &dst_pixel_color, to, m_bpp );

						size_t range = ( mode & AM_GRADIENT_TIGHTER ) ? 1 : 2;
						
						float p = 0.0f;
						
						if ( mode & AM_GRADIENT_LEFT ) {
							if ( mode & AM_GRADIENT_TOP ) {
								if ( ( dx + dy ) < ( cx + cy ) ) {
									p = ( (float)( ( cx + cy ) - ( dx + dy ) ) / ( w + h ) * range );
								}
							}
							else if ( mode & AM_GRADIENT_BOTTOM ) {
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
						else if ( mode & AM_GRADIENT_RIGHT ) {
							if ( mode & AM_GRADIENT_TOP ) {
								if ( ( cx + dy ) < ( dx + cy ) ) {
									p = ( (float)( ( dx + cy ) - ( cx + dy ) ) / ( w + h ) * range );
								}
							}
							else if ( mode & AM_GRADIENT_BOTTOM ) {
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
						else if ( mode & AM_GRADIENT_TOP ) {
							if ( dy < cy ) {
								p = (float)( cy - dy ) / h * range;
							}
						}
						else if ( mode & AM_GRADIENT_BOTTOM ) {
							if ( cy < dy ) {
								p = (float)( dy - cy ) / h * range;
							}
						}

						if ( pixel_alpha < 1.0f ) {
							p *= pixel_alpha;
						}

						pixel_color = (
							(uint8_t)( (float)( pixel_color & 0xff ) * p + (float)( dst_pixel_color & 0xff ) * ( 1.0f - p ) ) |
							(uint8_t)( (float)( pixel_color >> 8 & 0xff ) * p + (float)( dst_pixel_color >> 8 & 0xff ) * ( 1.0f - p ) ) << 8 |
							(uint8_t)( (float)( pixel_color >> 16 & 0xff ) * p + (float)( dst_pixel_color >> 16 & 0xff ) * ( 1.0f - p ) ) << 16 |
							(uint8_t)( (float)( pixel_color >> 24 & 0xff ) * p + (float)( dst_pixel_color >> 24 & 0xff ) * ( 1.0f - p ) ) << 24
						);

						memcpy( to, &pixel_color, m_bpp );
					}
					else {
						memcpy( to, from, m_bpp );
						if ( pixel_alpha < 1.0f ) {
							if ( mode & AM_MERGE ) {
								
								// TODO: refactor
								
								uint32_t pixel_color;
								memcpy( &pixel_color, from, m_bpp );
								uint32_t dst_pixel_color;
								memcpy( &dst_pixel_color, to, m_bpp );
								
								pixel_color =
									(uint8_t)( (float)( pixel_color & 0xff ) * pixel_alpha + (float)( dst_pixel_color & 0xff ) * ( 1.0f - pixel_alpha ) ) |
									(uint8_t)( (float)( pixel_color >> 8 & 0xff ) * pixel_alpha + (float)( dst_pixel_color >> 8 & 0xff ) * ( 1.0f - pixel_alpha ) ) << 8 |
									(uint8_t)( (float)( pixel_color >> 16 & 0xff ) * pixel_alpha + (float)( dst_pixel_color >> 16 & 0xff ) * ( 1.0f - pixel_alpha ) ) << 16 |
									(uint8_t)( (float)( pixel_color >> 24 & 0xff ) * pixel_alpha + (float)( dst_pixel_color >> 24 & 0xff ) * ( 1.0f - pixel_alpha ) ) << 24
								;
								
								//*((uint8_t*)( to ) + 3 ) = (uint8_t)floor( pixel_alpha * 0xff + ( 1.0f - pixel_alpha ) * *((uint8_t*)( to ) + 3 ) );
							}
							else {
								*((uint8_t*)( to ) + 3 ) = (uint8_t)floor( pixel_alpha * 0xff );
							}
						}
					}
				}
			}
		}
	}
}

void Texture::Rotate() {
	unsigned char *new_bitmap = (unsigned char*)malloc(m_bitmap_size);
	
	const size_t tmp = m_width;
	m_width = m_height;
	m_height = tmp;
	
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			memcpy(
				ptr( new_bitmap, ( y * m_width + x ) * m_bpp, m_bpp ),
				ptr( m_bitmap, ( x * m_height + y ) * m_bpp, m_bpp ),
				m_bpp
			);
		}
	}
	
	free( m_bitmap );
	m_bitmap = new_bitmap;
}

void Texture::FlipV() {
	unsigned char *new_bitmap = (unsigned char*)malloc(m_bitmap_size);
	
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			memcpy(
				ptr( new_bitmap, ( y * m_width + x ) * m_bpp, m_bpp ),
				ptr( m_bitmap, ( ( m_height - y - 1 ) * m_width + x ) * m_bpp, m_bpp ),
				m_bpp
			);
		}
	}
	
	free( m_bitmap );
	m_bitmap = new_bitmap;
}

void Texture::SetAlpha(const float alpha) {
	const uint8_t alpha_byte = alpha * 255;
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			*ptr( m_bitmap, ( y * m_width + x ) * m_bpp + 3, sizeof( alpha_byte ) ) = alpha_byte;
		}
	}
}

void Texture::SetContrast(const float contrast) {
	size_t i;
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			for (size_t b = 0 ; b < 3 ; b++) {
				i = ( y * m_width + x ) * m_bpp + b;
				*ptr( m_bitmap, i, sizeof( unsigned char ) ) = (unsigned char) floor(std::fmin(255, (float)m_bitmap[i] * contrast));
			}
		}
	}
}

const Buffer Texture::Serialize() const {
	Buffer buf;
	
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

void Texture::Unserialize( Buffer buf ) {
	
	m_name = buf.ReadString();
	size_t width = buf.ReadInt();
	ASSERT( width == m_width, "texture read width mismatch ( " + std::to_string( width ) + " != " + std::to_string( m_width ) + " )" );
	size_t height = buf.ReadInt();
	ASSERT( height == m_height, "texture read height mismatch ( " + std::to_string( height ) + " != " + std::to_string( m_height ) + " )" );
	
	m_aspect_ratio = buf.ReadFloat();
	
	m_bpp = buf.ReadInt();
	ASSERT( m_bpp == 4, "invalid bpp" );
	
	m_bitmap_size = buf.ReadInt();
	
	if ( m_bitmap ) {
		free( m_bitmap );
	}
	m_bitmap = (unsigned char*)buf.ReadData( m_bitmap_size );
	
	m_is_tiled = buf.ReadBool();
	
}

}
