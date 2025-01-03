#pragma once

#include <vector>

#include "types/Vec3.h"

namespace game {
namespace frontend {

class Game;

namespace tile {
class Tile;
}

namespace ui_legacy {
class ObjectsListItem;
class ObjectPreview;
}

class TileObject {
public:
	enum tile_object_type_t {
		TOT_NONE,
		TOT_UNIT,
		TOT_BASE,
	};
	TileObject( const tile_object_type_t type, tile::Tile* tile );

	void UpdateFromTile();

	const tile_object_type_t GetType() const;

	virtual void* CreateOnBottomBarList( ui_legacy::ObjectsListItem* element ) const = 0;
	virtual void DestroyOnBottomBarList( ui_legacy::ObjectsListItem* element, void* state ) const = 0;
	virtual void* CreateOnBottomBarPreview( ui_legacy::ObjectPreview* element ) const = 0;
	virtual void DestroyOnBottomBarPreview( ui_legacy::ObjectPreview* element, void* state ) const = 0;
	virtual const bool OnBottomBarListActivate( Game* game ) = 0;

protected:

	const tile_object_type_t m_type;
	tile::Tile* m_tile = nullptr;

	virtual void SetRenderCoords( const types::Vec3& coords ) = 0;

private:

};

}
}