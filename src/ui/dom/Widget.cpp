#include "Widget.h"

#include "ui/UI.h"
#include "types/texture/Texture.h"
#include "ui/geometry/Geometry.h"

namespace ui {
namespace dom {

Widget::Widget( DOM_ARGS )
	: Surface( DOM_ARGS_PASS, "widget" ) {

	Property(
		GSE_CALL, "type", gse::Value::T_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto& str = ( (gse::value::String*)v )->value;
			const auto type = m_ui->GetWidgetTypeByString( GSE_CALL, str );
			Enable( type );
		},
		[ this ]( GSE_CALLABLE ) {
			Disable();
		}
	);

}

void Widget::Show() {
	if ( !m_is_visible ) {
		Surface::Show();
		if ( m_type != WT_NONE ) {
			Attach();
		}
	}
}

void Widget::Hide() {
	if ( m_is_visible ) {
		if ( m_type != WT_NONE ) {
			Detach();
		}
		Surface::Hide();
	}
}

void Widget::Enable( const widget_type_t type ) {
	ASSERT( type != WT_NONE, "widget type is none" );
	if ( type != m_type ) {
		if ( m_is_visible ) {
			Detach();
		}
		m_type = type;
		if ( m_is_visible ) {
			Attach();
		}
	}
}

void Widget::Disable() {
	if ( m_type != WT_NONE ) {
		if ( m_is_visible ) {
			Detach();
		}
		m_type = WT_NONE;
	}
}

void Widget::Attach() {
	if ( !m_is_attached ) {
		if ( m_is_visible ) {
			m_ui->AttachWidget( this, m_type );
		}
		m_is_attached = true;
	}
}

void Widget::Detach() {
	if ( m_is_attached ) {
		if ( m_is_visible ) {
			m_ui->DetachWidget( this );
		}
		m_is_attached = false;
	}
}

types::texture::Texture* const Widget::GetTexture() {
	CreateTexture();
	m_background.texture->Resize( m_geometry->GetWidth(), m_geometry->GetHeight() );
	ASSERT( m_background.is_texture_owned, "can't render into texture that is not owned" );
	return m_background.texture;
}

}
}
