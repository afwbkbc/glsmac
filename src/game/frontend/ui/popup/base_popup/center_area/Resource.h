#pragma once

#include "CenterAreaPage.h"

namespace types::texture {
class Texture;
}

namespace ui::object {
class Mesh;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace tile {
class Tile;
}

namespace ui {
namespace popup {
namespace base_popup {
namespace center_area {

CLASS( Resource, CenterAreaPage )

	Resource( CenterArea* center_area );

	void Create() override;
	void Destroy() override;

private:
	friend class CenterArea;
	void Update( base::Base* base );

	typedef std::vector< ::ui::object::Mesh* > tile_preview_t;
	std::vector< tile_preview_t > m_tile_previews = {};

	std::unordered_map< const tile::Tile*, types::Vec2< int8_t > > m_resource_tiles = {};

	void UpdateResourceTiles( base::Base* base );
	void PreviewTile( const tile::Tile* tile, const types::Vec2< int8_t > pos );
	void HideTilePreviews();

};

}
}
}
}
}
}
