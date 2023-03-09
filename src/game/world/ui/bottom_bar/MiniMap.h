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
	
	void SetMinimapSelection( const Vec2< float > position_percents, const Vec2< float > zoom );
	void ClearMinimapSelection();
	
	const bool IsMouseOver() const;
	const bool IsMouseDragging() const;
	
private:
	types::Texture* m_texture = nullptr;
	bool m_is_dragging = false;
	bool m_is_mouse_over = false;
	types::Texture* m_map_selection_texture = nullptr;
	
	struct {
		const UIEventHandler* mouse_up = nullptr;
		const UIEventHandler* mouse_move = nullptr;
	} m_handlers;
	
	Vec2< ssize_t > m_last_mouse_position = { 0, 0 }; // TODO: fix mouse coordinates passing with mouse scroll
	
	object::Button* m_turn_complete_button = nullptr;
	
	object::Mesh* m_map_surface = nullptr;
	object::Mesh* m_map_selection = nullptr;
	
	object::Section* m_bottom_bar = nullptr;
	struct {
		object::Label* mission_year = nullptr;
		object::Label* energy = nullptr;
	} m_bottom_bar_labels = {};
	
};
	
}
}
}
