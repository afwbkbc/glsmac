#include "Widget.h"

#include "ui/UI.h"
#include "types/texture/Texture.h"
#include "ui/geometry/Geometry.h"
#include "ui/geometry/Rectangle.h"

namespace ui {
namespace dom {

Widget::Widget( DOM_ARGS )
	: Area( DOM_ARGS_PASS, "widget" ) {

	Property(
		GSE_CALL, "type", gse::VT_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto& str = ( (gse::value::String*)v )->value;
			const auto type = m_ui->GetWidgetTypeByString( GSE_CALL, str );
			if ( m_data ) {
				m_ui->ValidateWidgetData( GSE_CALL, type, m_data );
			}
			Enable( type );
		},
		[ this ]( GSE_CALLABLE ) {
			Disable();
		}
	);

	Property(
		GSE_CALL, "data", gse::VT_OBJECT, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			if ( m_type ) {
				m_ui->ValidateWidgetData( GSE_CALL, m_type, (gse::value::Object*)v );
			}
			SetData( (gse::value::Object*)v );
			// TODO: redraw
		},
		[ this ]( GSE_CALLABLE ) {
			if ( m_type ) {
				m_ui->ValidateWidgetData( GSE_CALL, m_type, nullptr );
			}
			SetData( nullptr );
			// TODO: redraw
		}
	);

	m_geometry->m_on_resize = [ this ]( const size_t width, const size_t height ) {
		for ( const auto& it : m_textures ) {
			it.second->Resize( width, height );
		}
	};
}

void Widget::Show() {
	if ( !m_is_visible ) {
		Area::Show();
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
		Area::Hide();
	}
}

void Widget::Destroy( GSE_CALLABLE ) {
	if ( m_on_widget_remove ) {
		m_on_widget_remove( this );
	}
	Area::Destroy( GSE_CALL );
}

void Widget::SetTexture( types::texture::Texture* const texture, const size_t index ) {
	ASSERT( m_textures.find( index ) == m_textures.end(), "texture id " + std::to_string( index ) + " already set" );
	m_textures.insert( { index, texture } );
}

void Widget::SetActor( scene::actor::Actor* const actor, const size_t index ) {
	ASSERT( m_actors.find( index ) == m_actors.end(), "actor id " + std::to_string( index ) + " already set" );
	m_actors.insert( { index, actor } );
	Actor( actor );
}

void Widget::Clear() {
	m_geometry->AsRectangle()->Clear();
	ClearActors();
	m_actors.clear();
	for ( const auto& it : m_textures ) {
		DELETE( it.second );
	}
	m_textures.clear();
}

types::texture::Texture* const Widget::GetTexture( const size_t index ) const {
	ASSERT( m_textures.find( index ) != m_textures.end(), "texture id " + std::to_string( index ) + " not set" );
	return m_textures.at( index );
}

void Widget::OnUpdate( const f_widget_update_t& on_widget_update ) {
	if ( !m_on_widget_update ) { // TODO: refactor
		ASSERT( !m_on_widget_update, "m_on_widget_update already set" );
		m_on_widget_update = on_widget_update;
	}
	if ( m_type && m_data ) {
		UpdateWidget();
	}
}

void Widget::OnRemove( const f_widget_remove_t& on_widget_remove ) {
	m_on_widget_remove = on_widget_remove;
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

void Widget::SetData( gse::value::Object* const data ) {
	if ( data != m_data ) {
		m_data = data;
		m_data_update_needed = true;
		if ( m_type != WT_NONE && m_is_visible ) {
			m_ui->SetWidgetData( this, m_data );
			UpdateWidget();
			m_data_update_needed = false;
		}
	}
}

void Widget::Attach() {
	if ( !m_is_attached ) {
		if ( m_is_visible ) {
			m_ui->AttachWidget( this, m_type );
			if ( m_data_update_needed ) {
				m_ui->SetWidgetData( this, m_data );
				m_data_update_needed = false;
			}
			else {
				UpdateWidget();
			}
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

void Widget::UpdateWidget() {
	if ( m_on_widget_update ) {
		m_on_widget_update( this, m_data );
	}
}

}
}
