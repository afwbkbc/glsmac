#pragma once

#include <string>
#include <vector>

#include "types/Vec3.h"

namespace task {
namespace game {

namespace sprite {
class InstancedSprite;
class InstancedSpriteManager;
}

namespace text {

class InstancedFont;

class InstancedText {
public:
	InstancedText( const std::string& text, InstancedFont* font );
	~InstancedText();

	void ShowAt( const types::Vec3& coords );
	void Hide();

private:
	const std::string m_text = "";
	std::vector< sprite::InstancedSprite* > m_text_sprites = {};
	std::vector< size_t > m_instance_ids = {};

};

}
}
}
