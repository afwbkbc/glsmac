#include "Listview.h"

#include "ui/geometry/Geometry.h"
#include "Scrollbar.h"
#include "gse/value/Bool.h"

namespace ui {
namespace dom {

Listview::Listview( DOM_ARGS_T )
	: Scrollview( DOM_ARGS_PASS_T, true ) {

	PROPERTY( "itemsize", gse::value::Int, 0, SetItemSize );
	Property(
		GSE_CALL, "scroll_type", gse::Value::T_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto value = ( (gse::value::String*)v )->value;
			scroll_type_t st;
			if ( value == "vertical" ) {
				st = ST_VERTICAL;
			}
			else if ( value == "horizontal" ) {
				st = ST_HORIZONTAL;
			}
			else {
				GSE_ERROR( gse::EC.TYPE_ERROR, "Unknown scroll_type: " + value + ". Available: vertical horizontal" );
			}
			SetScrollType( st );
		},
		[ this ]( GSE_CALLABLE ) {
			if ( m_scroll_type != ST_VERTICAL ) {
				SetScrollType( ST_VERTICAL );
			}
		}
	);
	PROPERTY( "autoscroll", gse::value::Bool, 0, SetAutoScroll );

	m_on_add_child = [ this ]( Object* const obj, const bool is_from_factory ) {
		if ( is_from_factory ) {
			ASSERT( m_children.find( obj->m_id ) == m_children.end(), "child already added" );
			const auto offset = m_children.size() * m_itemsize;
			m_children.insert( { obj->m_id, obj } );
			switch ( m_scroll_type ) {
				case ST_VERTICAL: {
					obj->GetGeometry()->SetTop( offset );
					break;
				}
				case ST_HORIZONTAL: {
					obj->GetGeometry()->SetLeft( offset );
					break;
				}
				default:
					ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
			}
			if ( m_autoscroll ) {
				bool was_visible = false;
				size_t oldmax = 0;
				switch ( m_scroll_type ) {
					case ST_VERTICAL: {
						was_visible = m_vscroll->m_is_visible;
						oldmax = m_vscroll->m_max;
						break;
					}
					case ST_HORIZONTAL: {
						was_visible = m_hscroll->m_is_visible;
						oldmax = m_hscroll->m_max;
						break;
					}
					default:
						ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
				}
				m_on_scrollview_resize = [ this, offset, was_visible, oldmax ]() {
					switch ( m_scroll_type ) {
						case ST_VERTICAL: {
							if ( m_vscroll->m_is_visible && ( !was_visible || m_vscroll->m_value == oldmax ) ) {
								m_vscroll->SetValueRaw( offset + m_itemsize, true );
							}
							break;
						}
						case ST_HORIZONTAL: {
							if ( m_hscroll->m_is_visible && ( !was_visible || m_hscroll->m_value == oldmax ) ) {
								m_hscroll->SetValueRaw( offset + m_itemsize, true );
							}
							break;
						}
						default:
							ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
					}
					m_on_scrollview_resize = nullptr;
				};
			}
		}
	};
	m_on_remove_child = [ this ]( Object* const obj ) {
		auto it = m_children.find( obj->m_id );
		if ( it != m_children.end() ) {
			switch ( m_scroll_type ) {
				case ST_VERTICAL: {
					while ( ( ++it ) != m_children.end() ) {
						auto* g = it->second->GetGeometry();
						g->SetTop( g->GetTop() - m_itemsize );
					}
					break;
				}
				case ST_HORIZONTAL: {
					while ( ( ++it ) != m_children.end() ) {
						auto* g = it->second->GetGeometry();
						g->SetLeft( g->GetLeft() - m_itemsize );
					}
					break;
				}
				default:
					ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
			}
			m_children.erase( obj->m_id );
		}
	};
}

void Listview::SetItemSize( GSE_CALLABLE, const int itemsize ) {
	if ( itemsize < 0 ) {
		GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Item size can't be negative" );
	}
	if ( itemsize != m_itemsize ) {
		m_itemsize = itemsize;
		Realign();
	}
}

void Listview::SetAutoScroll( GSE_CALLABLE, const bool value ) {
	if ( value != m_autoscroll ) {
		m_autoscroll = value;
	}
}

void Listview::SetScrollType( const scroll_type_t type ) {
	if ( type != m_scroll_type ) {
		m_scroll_type = type;
		Realign();
	}
}

void Listview::Realign() {

}

}
}
