#pragma once

#include <unordered_map>
#include <mutex>

#include "common/Common.h"

#include "ui/Types.h"

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
		const Game* const game,
		ui::UI* const ui,
		const ui::widget_type_t type,
		const std::string& str,
		const ui::widget_data_config_t& data_config
	);
	virtual ~Widget();

protected:
	const Game* const m_game;
	ui::UI* const m_ui;
	const ui::widget_type_t m_type;

	// TODO: remove this
	void WithWidget( const ui::f_with_widget_t& f ) const;
#define WITH_WIDGET( ... ) WithWidget( F_WITH_WIDGET( __VA_ARGS__ )

	virtual void Register( ui::dom::Widget* const widget ) = 0;

};

}
}
}
