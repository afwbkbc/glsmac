#pragma once

#include "Section.h"

#include "ui/object/Button.h"
#include "ui/object/Mesh.h"
#include "ui/object/Section.h"
#include "ui/object/Label.h"

#include "types/Texture.h"

namespace game {
namespace world {
namespace ui {

CLASS( MiniMap, Section )

	MiniMap( World* world );
	
	void Create();
	void Destroy();

	void SetMinimapTexture( types::Texture* texture );
	
private:
	types::Texture* m_texture = nullptr;
	bool m_is_dragging = false;
	
	object::Button* m_turn_complete_button = nullptr;
	
	object::Mesh* m_map_surface = nullptr;
	
	object::Section* m_bottom_bar = nullptr;
	struct {
		object::Label* mission_year = nullptr;
		object::Label* energy = nullptr;
	} m_bottom_bar_labels = {};
	
};
	
}
}
}
