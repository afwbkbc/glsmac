#pragma once

#include <vector>

#include "BBSection.h"
#include "ui/object/Section.h"

#include "task/game/Types.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitPreview, BBSection )

	UnitPreview( Game* game );

	void Create() override;
	void Destroy() override;

	void PreviewUnit( const unit_data_t* unit_data );
	void HideUnitPreview();
	void HideUnitPreview( const unit_data_t* unit_data );

private:
	const unit_data_t* m_unit_data = nullptr;

	struct {
		object::Mesh* unit = nullptr;
		object::Mesh* badge = nullptr;
		object::Mesh* healthbar = nullptr;
	} m_sprites;

	std::vector< ::ui::object::Label* > m_labels = {};

};

}
}
}
