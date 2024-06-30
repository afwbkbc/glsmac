#pragma once

#include "task/game/ui/Section.h"

namespace types::mesh {
class Mesh;
}

namespace ui::object {
class Label;
class Mesh;
class Panel;
}

namespace task {
namespace game {

namespace unit {
class Unit;
}

namespace ui {

class UnitsList;
class UnitPreview;

CLASS( UnitsListItem, Section )

	UnitsListItem( Game* game, UnitsList* units_list, unit::Unit* unit );

	void Create() override;
	void Destroy() override;

private:
	friend class UnitsList;
	void SelectUnit();
	void DeselectUnit();
	const unit::Unit* GetUnit() const;

private:
	UnitsList* m_units_list;
	unit::Unit* m_unit;

	struct sprite_t {
		types::mesh::Mesh* mesh = nullptr; // save mesh to be able to preview units
		::ui::object::Mesh* obj = nullptr; // this will get copy of mesh
	};
	struct {
		sprite_t unit = {};
		sprite_t badge = {};
		sprite_t healthbar = {};
	} m_sprites;
	::ui::object::Label* m_label = nullptr;

	::ui::object::Panel* m_selection_frame = nullptr;
};

}
}
}
