#pragma once

#include <vector>

#include "game/frontend/ui/bottom_bar/BBSection.h"

namespace ui::object {
class ScrollView;
}

namespace game {
namespace frontend {

class TileObject;

namespace ui {

class ObjectPreview;
class ObjectsListItem;

CLASS( ObjectsList, BBSection )

	ObjectsList( Game* game, ObjectPreview* object_preview );

	void Create() override;
	void Destroy() override;

	void ClearObjects();
	void ListObjects( const std::vector< TileObject* >& objects, const size_t selected_unit_id );

private:
	friend class ObjectsListItem;
	void PreviewObject( TileObject* object );
	void HideObjectPreview( TileObject* object );
	void SelectObject( TileObject* object, const bool activate_object );

private:
	::ui::object::ScrollView* m_body = nullptr;
	ObjectPreview* m_object_preview;
	std::unordered_map< TileObject*, ObjectsListItem* > m_items = {};
	TileObject* m_selected_object = nullptr;
	TileObject* m_previewing_object = nullptr;
};

}
}
}
