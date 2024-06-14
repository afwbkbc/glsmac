#pragma once

#include <string>

namespace types {
class Font;
namespace texture {
class Texture;
}
}

namespace task {
namespace game {
namespace text {

class InstancedFont {
public:
	InstancedFont( const types::Font* font );
	~InstancedFont();

	const std::string& GetFontName() const;

private:
	const std::string m_name = "";

	types::texture::Texture* m_texture = nullptr;
};

}
}
}
