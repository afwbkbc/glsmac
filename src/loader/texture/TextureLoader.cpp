#include "TextureLoader.h"

#include "types/texture/Texture.h"

namespace loader {
namespace texture {

// different files use different color for transparency
static const auto s_tc_pink = types::Color::RGB( 255, 0, 255 );
static const auto s_tc_purple = types::Color::RGB( 152, 24, 228 );
static const auto s_tc_darkpurple = types::Color::RGB( 100, 16, 156 );
static const auto s_tc_aqua = types::Color::RGB( 24, 184, 228 );
static const std::unordered_map< std::string, TextureLoader::transparent_colors_t > s_tcs = {
	{
		"texture.pcx",
		{
			types::Color::RGB( 125, 0, 128 ),
		}
	},
	{
		"interface.pcx",
		{
			s_tc_pink,
		}
	},
	{
		"moon1.pcx",
		{
			s_tc_pink,
		}
	},
	{
		"moon2.pcx",
		{
			s_tc_pink,
		}
	},
	{
		"moon3.pcx",
		{
			s_tc_pink,
		}
	},
	{
		"Icons.pcx",
		{
			s_tc_purple,
		}
	},
	{
		"console_x.pcx",
		{
			s_tc_pink,
		}
	},
	{
		"console2_A.pcx",
		{
			s_tc_darkpurple,
		}
	},
	{
		"console_x2_a.pcx",
		{
			s_tc_pink,
		}
	},
	{
		"Jackal.pcx",
		{
			s_tc_darkpurple,
		}
	},
	{
		"ter1.pcx",
		{
			s_tc_purple,
			s_tc_darkpurple, // tile markings
			s_tc_aqua, // borders
			types::Color::RGB( 253, 189, 118 ), // shadows
		}
	},
	{
		"Units.pcx",
		{
			s_tc_purple,
			s_tc_darkpurple, // tile markings
			s_tc_aqua, // borders
		}
	},
	{
		"flags.pcx",
		{
			types::Color::RGB( 124, 124, 124 ),
			s_tc_aqua, // borders
		}
	},
	{
		"xi.pcx",
		{
			types::Color::RGB( 0, 67, 255 ),
			types::Color::RGB( 27, 187, 231 ), // borders
		}
	},
	{
		"XF.pcx",
		{
			types::Color::RGB( 0, 0, 0 ),
			types::Color::RGB( 155, 27, 231 ), // tile
			types::Color::RGB( 27, 187, 231 ), // borders
		}
	}
};
static const TextureLoader::transparent_colors_t s_no_transparent_colors = {};

const TextureLoader::transparent_colors_t& TextureLoader::GetTCs( const std::string& name ) {
	const auto& transparent_colors_it = s_tcs.find( name );
	if ( transparent_colors_it != s_tcs.end() ) {
		return transparent_colors_it->second;
	}
	else {
		return s_no_transparent_colors;
	}
}

types::texture::Texture* TextureLoader::LoadTexture( const std::string& name ) {
	const transparent_colors_t colors_old = m_transparent_colors;
	m_transparent_colors = GetTCs( name );
	types::texture::Texture* result = LoadTextureImpl( name );
	m_transparent_colors = colors_old;
	return result;
}

types::texture::Texture* TextureLoader::LoadTexture( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) {
	const transparent_colors_t colors_old = m_transparent_colors;
	m_transparent_colors = GetTCs( name );
	types::texture::Texture* result = LoadTextureImpl( name, x1, y1, x2, y2, flags, value );
	m_transparent_colors = colors_old;
	return result;
}

types::texture::Texture* TextureLoader::GetColorTexture( const types::Color& color ) {
	const types::Color::rgba_t c = color.GetRGBA();
	const auto& it = m_color_textures.find( c );
	if ( it != m_color_textures.end() ) {
		return it->second;
	}
	auto* texture = types::texture::Texture::FromColor( color );
	m_color_textures[ c ] = texture;
	return texture;
}

}
}
