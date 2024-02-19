#pragma once

#include "../../Section.h"

#include "task/game/Types.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitsListItem, Section )

	UnitsListItem( Game* game, const unit_data_t* unit_data );

	void Create() override;
	void Destroy() override;

private:
	const unit_data_t* m_unit_data;
	object::Mesh* m_preview = nullptr;

};

}
}
}
