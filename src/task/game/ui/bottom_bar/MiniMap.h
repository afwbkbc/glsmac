#pragma once

#include "BBSection.h"

#include "ui/object/Button.h"
#include "ui/object/Mesh.h"
#include "ui/object/Section.h"
#include "ui/object/Label.h"

#include "types/Texture.h"

namespace task {
namespace game {
namespace ui {

CLASS( MiniMap, BBSection )

	MiniMap( Game* game );
	
	void Create() override;
	void Destroy() override;

	void SetMinimapTexture( types::Texture* texture );
	
	void SetMinimapSelection( const Vec2< float > position_percents, const Vec2< float > zoom );
	void ClearMinimapSelection();
	
	const bool IsMouseDragging() const;
	
private:
	types::Texture* m_texture = nullptr;
	bool m_is_dragging = false;
	bool m_is_mouse_over = false;
	types::Texture* m_map_selection_texture = nullptr;
	Vec2< ssize_t > m_last_selection_size = { 0, 0 };
	
	struct {
		const UIEventHandler* mouse_up = nullptr;
		const UIEventHandler* mouse_move = nullptr;
	} m_handlers;
	
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
