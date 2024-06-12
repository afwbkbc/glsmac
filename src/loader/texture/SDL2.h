#pragma once

#include <unordered_map>

#include <SDL_image.h>

#include "TextureLoader.h"

namespace loader {
namespace texture {

CLASS( SDL2, TextureLoader )
	virtual ~SDL2();

	void Start() override;
	void Stop() override;
	void Iterate() override;

protected:

	types::texture::Texture* LoadTextureImpl( const std::string& filename ) override;
	types::texture::Texture* LoadTextureImpl( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) override;

	// cache all textures for future use
	typedef std::unordered_map< std::string, types::texture::Texture* > texture_map_t;
	texture_map_t m_textures = {};
	texture_map_t m_subtextures = {};

private:
	void FixTransparency( types::texture::Texture* texture ) const;
	void FixYellowShadows( types::texture::Texture* texture ) const;
	void FixTexture( types::texture::Texture* texture ) const;

};

}
}
