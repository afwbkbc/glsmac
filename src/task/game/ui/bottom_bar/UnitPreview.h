#pragma once

#include <vector>

#include "BBSection.h"

namespace ui::object {
class Section;
class Label;
class Mesh;
}

namespace task {
namespace game {

class Unit;

namespace ui {

CLASS( UnitPreview, BBSection )

	UnitPreview( Game* game );

	void Create() override;
	void Destroy() override;

	void PreviewUnit( const Unit* unit );
	void HideUnitPreview();

private:
	const Unit* m_unit = nullptr;

	struct {
		::ui::object::Mesh* unit = nullptr;
		::ui::object::Mesh* badge = nullptr;
		::ui::object::Mesh* healthbar = nullptr;
	} m_sprites;

	std::vector< ::ui::object::Label* > m_labels = {};

};

}
}
}
