#pragma once

#include "common/Common.h"

#include "ui/Types.h"

namespace ui {
class UI;
namespace dom {
class Widget;
}
}

namespace game {
namespace frontend {
namespace widget {

CLASS( Widget, common::Class )
public:
	Widget( ui::UI* const ui, const ui::widget_type_t type, const std::string& str );
	virtual ~Widget();

protected:

	void WithWidget( const ui::f_with_widget_t& f ) const;
#define WITH_WIDGET( ... ) WithWidget( F_WITH_WIDGET( __VA_ARGS__ )

	virtual void Register( ui::dom::Widget* const widget ) = 0;

private:
	ui::UI* const m_ui;
	const ui::widget_type_t m_type;

};

}
}
}
