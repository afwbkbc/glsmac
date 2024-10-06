#pragma once

#include "game/frontend/ui/Section.h"

namespace types::mesh {
class Mesh;
}

namespace ui::object {
class Label;
class Mesh;
class Panel;
}

namespace game {
namespace frontend {

class TileObject;

namespace ui {

class ObjectsList;
class ObjectPreview;

CLASS( ObjectsListItem, Section )

	ObjectsListItem( Game* game, ObjectsList* objects_list, TileObject* object );

	void Create() override;
	void Destroy() override;

private:
	friend class ObjectsList;
	void SelectObject();
	void DeselectObject();
	const TileObject* GetObject() const;

private:
	ObjectsList* m_objects_list;
	TileObject* m_object;

	void* m_ui_state = nullptr;

	::ui::object::Label* m_label = nullptr;

	::ui::object::Panel* m_selection_frame = nullptr;
};

}
}
}
