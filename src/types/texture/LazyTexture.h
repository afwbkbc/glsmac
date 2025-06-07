#pragma once

#include <string>

namespace loader::texture {
class TextureLoader;
}

namespace types {
namespace texture {

class Texture;

class LazyTexture {
public:
	LazyTexture( loader::texture::TextureLoader* const texture_loader, const std::string& filename );
	~LazyTexture();

	const std::string& GetFilename() const;

	Texture* const Get();

private:
	loader::texture::TextureLoader* const m_texture_loader;
	const std::string m_filename;

	Texture* m_texture = nullptr;
	bool m_tried_and_failed = false;
};

}
}
