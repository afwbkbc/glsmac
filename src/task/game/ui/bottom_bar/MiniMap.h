#pragma once

#include "BBSection.h"

namespace types::texture {
class Texture;
}

namespace ui::object {
class Mesh;
class Section;
class Label;
}

namespace task {
namespace game {
namespace ui {

CLASS( MiniMap, BBSection )

	MiniMap( Game* game );

	void Create() override;
	void Destroy() override;

	void SetMinimapTexture( types::texture::Texture* texture );

	void SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom );
	void ClearMinimapSelection();

	const bool IsMouseDragging() const;

private:
	types::texture::Texture* m_texture = nullptr;
	bool m_is_dragging = false;
	types::texture::Texture* m_map_selection_texture = nullptr;
	types::Vec2< ssize_t > m_last_selection_size = {
		0,
		0
	};

	struct {
		const ::ui::event::UIEventHandler* mouse_up = nullptr;
		const ::ui::event::UIEventHandler* mouse_move = nullptr;
	} m_handlers;

	::ui::object::Mesh* m_map_surface = nullptr;
	::ui::object::Mesh* m_map_selection = nullptr;

	::ui::object::Section* m_bottom_bar = nullptr;
	struct {
		::ui::object::Label* mission_year = nullptr;
		::ui::object::Label* energy = nullptr;
	} m_bottom_bar_labels = {};

};

}
}
}
