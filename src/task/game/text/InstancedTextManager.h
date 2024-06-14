#pragma once

#include "base/Base.h"

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
namespace text {

class InstancedFont;
class InstancedText;

CLASS( InstancedTextManager, ::base::Base )
public:

	InstancedTextManager( scene::Scene* scene );
	~InstancedTextManager();

	InstancedFont* GetInstancedFont( const types::Font* font );

	InstancedText* CreateInstancedText(
		const std::string& text,
		InstancedFont* font,
		const types::Color& color,
		const z_level_t z_level,
		const float z_index_adjustment = 0.0f
	);

private:

	scene::Scene* m_scene = nullptr;

	std::unordered_map< const types::Font*, InstancedFont* > m_instanced_fonts = {};

};

}
}
}
