#pragma once

#include "../../Section.h"

#include "../UnitPreview.h"
#include "task/game/Types.h"
#include "task/game/Unit.h"
#include "ui/object/Label.h"

namespace task {
namespace game {
namespace ui {

class UnitsList;

CLASS( UnitsListItem, Section )

	UnitsListItem( Game* game, UnitsList* units_list, Unit* unit );

	void Create() override;
	void Destroy() override;

private:
	friend class UnitsList;
	void SelectUnit();
	void DeselectUnit();
	const Unit* GetUnit() const;

private:
	UnitsList* m_units_list;
	Unit* m_unit;

	struct sprite_t {
		types::mesh::Mesh* mesh = nullptr; // save mesh to be able to preview units
		object::Mesh* obj = nullptr; // this will get copy of mesh
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
