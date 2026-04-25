#pragma once

#include "Widget.h"

#include "types/Vec2.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {
namespace widget {

CLASS( Minimap, Widget )
public:

	Minimap( Game* const game, ui::UI* const ui );
	virtual ~Minimap();

	const types::Vec2< ui::coord_t > FindLargestArea() const;

	void Update( ui::dom::Widget* const widget, const void* const data ) override;

	void SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom );

protected:
	void Register( ui::dom::Widget* const widget ) override;

private:
	enum layer_index_t {
		LI_PREVIEW = 0,
		LI_SELECTION = 1,
	};

	bool m_is_dragging = false;

	types::texture::Texture* m_last_texture = nullptr;
	types::Vec2< float > m_last_position_percents = {};
	types::Vec2< float > m_last_zoom = {};

	void UpdateImpl( ui::dom::Widget* const widget, const types::texture::Texture* const texture );
	void SetMinimapSelectionImpl( ui::dom::Widget* const widget, const types::Vec2< float > position_percents, const types::Vec2< float > zoom );

};

}
}
}
