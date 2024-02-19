#pragma once

#include "../../Section.h"

#include "task/game/Types.h"

#include "ui/object/Label.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitsListItem, Section )

	UnitsListItem( Game* game, const unit_data_t* unit_data );

	void Create() override;
	void Destroy() override;

private:
	const unit_data_t* m_unit_data;

	struct {
		object::Mesh* unit = nullptr;
		object::Mesh* badge = nullptr;
		object::Mesh* healthbar = nullptr;
	} m_sprites;
	::ui::object::Label* m_label = nullptr;

};

}
}
}
