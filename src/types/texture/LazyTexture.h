#pragma once

#include <string>

#include "types/texture/Types.h"

namespace loader::texture {
class TextureLoader;
}

namespace types {
namespace texture {

class Texture;

class LazyTexture {
public:
	LazyTexture( loader::texture::TextureLoader* const texture_loader, const std::string& filename, const types::texture::texture_flag_t flags );
	~LazyTexture();

	const std::string& GetFilename() const;

	Texture* const Get();

private:
	loader::texture::TextureLoader* const m_texture_loader;
	const std::string m_filename;
	const types::texture::texture_flag_t m_flags;

	Texture* m_texture = nullptr;
	bool m_tried_and_failed = false;
};

}
}
