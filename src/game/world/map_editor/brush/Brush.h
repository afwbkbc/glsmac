#pragma once

#include <vector>

#include "base/Base.h"

#include "../MapEditor.h"

namespace game {
namespace world {
	class World;
namespace map_editor {
namespace brush {

CLASS( Brush, base::Base )
	
	Brush( World* world, const MapEditor::brush_type_t type );

	const MapEditor::brush_type_t GetType() const;

	// return tiles that need to be drawn
	virtual const MapEditor::tiles_t Draw( map::Tile* center_tile ) = 0;
	
protected:
	const World* m_world = nullptr;
	const MapEditor::brush_type_t m_type = MapEditor::BT_NONE;
	
};
	
}
}
}
}
