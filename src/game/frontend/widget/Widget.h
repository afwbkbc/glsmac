#pragma once

#include <unordered_map>
#include <mutex>

#include "common/Common.h"

#include "ui/Types.h"
#include "types/Vec2.h"

namespace types {
namespace mesh {
class Render;
}
namespace texture {
class Texture;
}
}

namespace gse::value {
class Object;
}

namespace ui {
class UI;
namespace dom {
class Widget;
}
}

namespace game {
namespace frontend {

class Game;

namespace widget {

CLASS( Widget, common::Class )
public:
	Widget(
		Game* const game,
		ui::UI* const ui,
		const ui::widget_type_t type,
		const std::string& str,
		const ui::widget_data_config_t& data_config
	);
	virtual ~Widget();

	virtual void Update( ui::dom::Widget* const widget, const void* const data ) = 0;

protected:
	Game* const m_game;
	ui::UI* const m_ui;
	const ui::widget_type_t m_type;

	void AddMeshAndTexture(
		ui::dom::Widget* const widget,
		size_t index,
		const types::mesh::Render* const mesh,
		types::texture::Texture* const texture,
		const bool keep_tex = false,
		const types::Vec2< float >& scale = { 1.0f, 1.0f },
		const types::Vec2< float >& offset = { 0.0f, 0.0f }
	);

	// TODO: remove this
	void WithWidget( const ui::f_with_widget_t& f ) const;
#define WITH_WIDGET( ... ) WithWidget( F_WITH_WIDGET( __VA_ARGS__ )

	virtual void Register( ui::dom::Widget* const widget ) = 0;

};

}
}
}
