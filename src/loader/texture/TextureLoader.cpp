#include "TextureLoader.h"

#include <algorithm>

#include "types/texture/Texture.h"
#include "engine/Engine.h"
#include "resource/ResourceManager.h"

namespace loader {
namespace texture {

// different files use different color for transparency
static const auto s_tc_pink = types::Color::RGB( 255, 0, 255 );
static const auto s_tc_purple = types::Color::RGB( 152, 24, 228 );
static const auto s_tc_darkpurple = types::Color::RGB( 100, 16, 156 );
static const auto s_tc_aqua = types::Color::RGB( 24, 184, 228 );
static const auto s_tc_yellowshadow = types::Color::RGB( 253, 189, 118 );
static const auto s_tc_facborder = types::Color::RGB( 77, 156, 176 );
static const std::unordered_map< resource::resource_t, TextureLoader::transparent_colors_t > s_tcs = {
	{
		resource::PCX_TEXTURE,
		{
			types::Color::RGB( 125, 0, 128 ),
		}
	},
	{
		resource::PCX_INTERFACE,
		{
			s_tc_pink,
		}
	},
	{
		resource::PCX_MOON1,
		{
			s_tc_pink,
		}
	},
	{
		resource::PCX_MOON2,
		{
			s_tc_pink,
		}
	},
	{
		resource::PCX_MOON3,
		{
			s_tc_pink,
		}
	},
	{
		resource::PCX_ICONS,
		{
			s_tc_purple,
		}
	},
	{
		resource::PCX_CONSOLE_X,
		{
			s_tc_pink,
		}
	},
	{
		resource::PCX_CONSOLE2_A,
		{
			s_tc_darkpurple,
		}
	},
	{
		resource::PCX_CONSOLE_X2_A,
		{
			s_tc_pink,
		}
	},
	{
		resource::PCX_JACKAL,
		{
			s_tc_darkpurple,
		}
	},
	{
		resource::PCX_TER1,
		{
			s_tc_purple,
			s_tc_darkpurple, // tile markings
			s_tc_aqua, // borders
			s_tc_yellowshadow // shadows
		}
	},
	{
		resource::PCX_UNITS,
		{
			s_tc_purple,
			s_tc_darkpurple, // tile markings
			s_tc_aqua, // borders
		}
	},
	{
		resource::PCX_FLAGS,
		{
			types::Color::RGB( 124, 124, 124 ),
			s_tc_aqua, // borders
		}
	},
	{
		resource::PCX_XI,
		{
			types::Color::RGB( 0, 67, 255 ),
			types::Color::RGB( 27, 187, 231 ), // borders
		}
	},
	{
		resource::PCX_XF,
		{
			types::Color::RGB( 0, 0, 0 ),
			types::Color::RGB( 155, 27, 231 ), // tile
			types::Color::RGB( 27, 187, 231 ), // borders
		}
	},
	{
		resource::PCX_GAIANS,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_HIVE,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_UNIV,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_MORGAN,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_SPARTANS,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_BELIEVE,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_PEACE,
		{
			s_tc_pink,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_CYBORG,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_PIRATES,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_DRONE,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_ANGELS,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_FUNGBOY,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_CARETAKE,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
	{
		resource::PCX_USURPER,
		{
			s_tc_darkpurple,
			s_tc_yellowshadow,
			s_tc_facborder,
		},
	},
};
static const TextureLoader::transparent_colors_t s_no_transparent_colors = {};

const TextureLoader::transparent_colors_t& TextureLoader::GetTCs( const resource::resource_t res ) {
	const auto& transparent_colors_it = s_tcs.find( res );
	if ( transparent_colors_it != s_tcs.end() ) {
		return transparent_colors_it->second;
	}
	else {
		return s_no_transparent_colors;
	}
}

types::texture::Texture* TextureLoader::LoadTexture( const resource::resource_t res ) {
	const transparent_colors_t colors_old = m_transparent_colors;
	m_transparent_colors = GetTCs( res );
	auto* result = LoadTextureImpl( GetFilename( res ) );
	m_transparent_colors = colors_old;
	return result;
}
types::texture::Texture* TextureLoader::LoadCustomTexture( const std::string& filename ) {
	std::string key;
	key.resize( filename.size() );
	std::transform( filename.begin(), filename.end(), key.begin(), ::tolower );
	const auto res = g_engine->GetResourceManager()->GetResource( filename );
	transparent_colors_t colors_old;
	if ( res != resource::NONE ) {
		colors_old = m_transparent_colors;
		m_transparent_colors = GetTCs( res );
	}
	auto* result = LoadTextureImpl( GetCustomFilename( filename ) );
	if ( res != resource::NONE ) {
		m_transparent_colors = colors_old;
	}
	return result;
}

types::texture::Texture* TextureLoader::LoadTexture( const resource::resource_t res, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) {
	const transparent_colors_t colors_old = m_transparent_colors;
	m_transparent_colors = GetTCs( res );
	types::texture::Texture* result = LoadTextureImpl( GetFilename( res ), x1, y1, x2, y2, flags, value );
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
