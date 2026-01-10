#include "Widget.h"

#include "ui/UI.h"

namespace game {
namespace frontend {
namespace widget {

Widget::Widget( const Game* const game, ui::UI* const ui, const ui::widget_type_t type, const std::string& str )
	: m_game( game )
	, m_ui( ui )
	, m_type( type ) {
	m_ui->RegisterWidget(
		m_type, {
			str,
			std::bind( &Widget::Register, this, std::placeholders::_1 ),
			{}
		}
	);
}

Widget::~Widget() {
	m_ui->UnregisterWidget( m_type );
}

void Widget::WithWidget( const ui::f_with_widget_t& f ) const {
	m_ui->WithWidget( m_type, f );
}

}
}
}
