#pragma once

#include <vector>

#include "base/Base.h"

#include "../MapEditor.h"

namespace game {
namespace world {
	class World;
namespace map_editor {
namespace tool {

CLASS( Tool, base::Base )
	
	Tool( World* world, MapEditor::tool_type_t type );

	const MapEditor::tool_type_t GetType() const;

	// return tiles that were drawn on and need reload
	virtual const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) = 0;
	
protected:
	const World* m_world = nullptr;
	const MapEditor::tool_type_t m_type = MapEditor::TT_NONE;
	
};
	
}
}
}
}
