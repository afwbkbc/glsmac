#pragma once

#include <vector>
#include <unordered_map>

#include "base/Base.h"

#include "Types.h"

namespace game {
class Game;
namespace map {
class Tile;
}

namespace map_editor {

namespace tool {
class Tool;
}
namespace brush {
class Brush;
}

CLASS( MapEditor, base::Base )

	std::unordered_map< tool_type_t, tool::Tool* > m_tools = {};
	tool::Tool* m_active_tool = nullptr;
	void SelectTool( tool_type_t tool );
	const tool_type_t GetActiveToolType() const;

	std::unordered_map< brush_type_t, brush::Brush* > m_brushes = {};
	brush::Brush* m_active_brush = nullptr;
	void SelectBrush( brush_type_t brush );
	const brush_type_t GetActiveBrushType() const;

	MapEditor( Game* game );
	~MapEditor();

	const bool IsEnabled() const;

	const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ); // returns tiles that need reload

private:
	Game* m_game = nullptr;

	const tiles_t GetUniqueTiles( const tiles_t& tiles ) const;
};

}
}
