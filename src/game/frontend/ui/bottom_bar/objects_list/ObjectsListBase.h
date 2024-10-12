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

class ObjectsListItem;

CLASS( ObjectsListBase, BBSection )

	ObjectsListBase( Game* game );

	void Create() override;
	void Destroy() override;

	void ClearObjects();
	void ListObjects( const std::vector< TileObject* >& objects, const size_t selected_unit_id );

protected:
	TileObject* const GetSelectedObject() const;

	virtual bool OnObjectMouseOver( TileObject* const object );
	virtual bool OnObjectMouseOut( TileObject* const object );
	virtual bool OnObjectClick( TileObject* const object );

protected:
	friend class ObjectsListItem;
	void SelectObject( TileObject* object, const bool activate_object );
	bool ObjectMouseOver( TileObject* const object );
	bool ObjectMouseOut( TileObject* const object );

private:
	::ui::object::ScrollView* m_body = nullptr;
	std::unordered_map< TileObject*, ObjectsListItem* > m_items = {};
	TileObject* m_selected_object = nullptr;
	TileObject* m_hovered_object = nullptr;
};

}
}
}
