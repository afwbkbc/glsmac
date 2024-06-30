#pragma once

#include "common/Common.h"

#include "task/game/Types.h"

#include "types/Color.h"

namespace types {
class Font;
}

namespace scene {
class Scene;
}

namespace task {
namespace game {

namespace sprite {
class InstancedSpriteManager;
}

namespace text {

class InstancedFont;
class InstancedText;

CLASS( InstancedTextManager, common::Class )
public:

	InstancedTextManager( sprite::InstancedSpriteManager* ism );
	~InstancedTextManager();

	InstancedFont* GetInstancedFont( const types::Font* font );

	InstancedText* CreateInstancedText(
		const std::string& text,
		InstancedFont* font,
		const types::Color& color,
		const types::Color& shadow_color
	);

private:

	sprite::InstancedSpriteManager* m_ism = nullptr;

	std::unordered_map< const types::Font*, InstancedFont* > m_instanced_fonts = {};
	std::unordered_map< const types::Font*, InstancedText* > m_instanced_texts = {};

};

}
}
}
