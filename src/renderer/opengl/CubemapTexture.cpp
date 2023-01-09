#include "CubemapTexture.h"

#include "renderer/Renderer.h"

namespace renderer {
namespace opengl {

CubemapTexture::CubemapTexture( types::Texture *texture ) : Texture( texture ) {
	m_target = GL_TEXTURE_CUBE_MAP;
}

void CubemapTexture::Load() {

	Log( "Creating OpenGL texture ( cubemap )" );

	glGenTextures( 1, &m_texture_obj );
	glBindTexture( m_target, m_texture_obj );
	uint16_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

	GLint pixel_format;

	size_t bw = m_texture->m_width / 4;
	size_t bh = m_texture->m_height / 3;
	size_t block_positions[6][2] = {
		{ bw, bh },
		{ bw * 3, bh },
		{ bw, 0 },
		{ bw, bh * 2 },
		{ 0, bh },
		{ bw * 2, bh },
	};

	if ( m_texture->m_bpp == 3 )
		pixel_format = GL_RGB;
	else if ( m_texture->m_bpp == 4 )
		pixel_format = GL_RGBA;
	else
		throw RendererError( "Unsupported texture format ( invalid BPP )" );
	if ( ( m_texture->m_height * 4 % 3 ) || ( m_texture->m_height * 4 / 3 != m_texture->m_width ) )
		throw RendererError( "Unsupported cubemap texture format ( invalid dimensions )" );


	size_t bs = bw * bh * m_texture->m_bpp;

	auto *block_data = new unsigned char[ bs ];

	for (uint8_t i=0;i<6;i++) { // experimental, we assume GL_TEXTURE_CUBE_MAP_* are sequential

		size_t xpos, ypos;

		// TODO: optimize?
		for ( size_t y = 0 ; y < bh ; y++ )
			for ( size_t x = 0 ; x < bw ; x++ )
				for ( size_t bpp = 0 ; bpp < m_texture->m_bpp ; bpp++ ) {
					if ( i == 2 ) {
						xpos = bh - y - 1;
						ypos = x;
					}
					else if ( i == 3 ) {
						xpos = y;
						ypos = bw - x - 1 ;
					}
					else {
						xpos = x;
						ypos = y;
					}
					block_data[ ( y * bw + x ) * m_texture->m_bpp + bpp ] = m_texture->m_bitmap[ ( ( block_positions[i][1] + ypos ) * m_texture->m_width + ( block_positions[i][0] + xpos ) ) * m_texture->m_bpp + bpp ];
				}

		glTexImage2D( target, 0, GL_RGB, bw, bh, 0, pixel_format, GL_UNSIGNED_BYTE, block_data );
		target++;
	};

	delete[] block_data;

	glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	glBindTexture( m_target, 0 );
}

} /* namespace opengl */
} /* namespace renderer */
