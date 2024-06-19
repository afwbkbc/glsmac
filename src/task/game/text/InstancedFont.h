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

namespace sprite {
class InstancedSpriteManager;
class InstancedSprite;
}

namespace text {

class InstancedFont {
public:
	InstancedFont( sprite::InstancedSpriteManager* ism, const types::Font* font );
	~InstancedFont();

	const std::string& GetFontName() const;
	sprite::InstancedSprite* GetSymbolSprite( const char symbol ) const;

private:
	sprite::InstancedSpriteManager* m_ism = nullptr;
	const types::Font* m_font = nullptr;
	const std::string m_name = "";

	types::texture::Texture* m_texture = nullptr;
};

}
}
}
