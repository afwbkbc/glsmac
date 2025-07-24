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

	m_on_before_add_child = [ this ]( const bool is_from_factory ) {
		if ( is_from_factory && m_autoscroll ) {
			switch ( m_scroll_type ) {
				case ST_VERTICAL: {
					m_was_visible = m_vscroll->m_is_visible;
					m_oldmax = m_vscroll->m_max/* + m_padding - m_itemsize*/;
					break;
				}
				case ST_HORIZONTAL: {
					m_was_visible = m_hscroll->m_is_visible;
					m_oldmax = m_hscroll->m_max/* + m_padding - m_itemsize*/;
					break;
				}
				default:
					ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
			}
		}
	};

	m_on_add_child = [ this ]( Object* const obj, const bool is_from_factory ) {
		if ( is_from_factory ) {
			ASSERT( m_children.find( obj->m_id ) == m_children.end(), "child already added" );
			const auto offset = m_children.size() * m_itemsize;
			m_children.insert( { obj->m_id, obj } );
			auto* g = obj->GetGeometry();
			switch ( m_scroll_type ) {
				case ST_VERTICAL: {
					g->SetHeight( m_itemsize );
					g->SetTop( offset );
					break;
				}
				case ST_HORIZONTAL: {
					g->SetWidth( m_itemsize );
					g->SetLeft( offset );
					break;
				}
				default:
					ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
			}
			/*if ( m_autoscroll ) {
				m_on_scrollview_resize = [ this, offset ]() {
					switch ( m_scroll_type ) {
						case ST_VERTICAL: {
							if ( m_vscroll->m_is_visible && ( !m_was_visible || m_vscroll->m_value + 2 /* TODO: fix properly * / >= m_oldmax ) ) {
								m_vscroll->SetValueRaw( m_vscroll->m_max /* offset + m_itemsize* / );
							}
							else {
								Log( "VALUE = " + std::to_string( m_vscroll->m_value ) + " ; OLDMAX = " + std::to_string( m_oldmax ) + " MAX = " + std::to_string( m_vscroll->m_max ) );
							}
							break;
						}
						case ST_HORIZONTAL: {
							if ( m_hscroll->m_is_visible && ( !m_was_visible || m_hscroll->m_value == m_oldmax ) ) {
								m_hscroll->SetValueRaw( offset + m_itemsize );
							}
							break;
						}
						default:
							ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
					}
					m_on_scrollview_resize = nullptr;
				};
			}*/
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

void Listview::SetScrollType( const scroll_type_t type ) {
	if ( type != m_scroll_type ) {
		m_scroll_type = type;
		Realign();
	}
}

void Listview::Realign() {
	coord_t offset = 0;
	switch ( m_scroll_type ) {
		case ST_VERTICAL: {
			for ( const auto& it : m_children ) {
				auto* g = it.second->GetGeometry();
				g->SetHeight( m_itemsize );
				g->SetTop( offset );
				offset += m_itemsize;
			}
			break;
		}
		case ST_HORIZONTAL: {
			for ( const auto& it : m_children ) {
				auto* g = it.second->GetGeometry();
				g->SetWidth( m_itemsize );
				g->SetLeft( offset );
				offset += m_itemsize;
			}
			break;
		}
		default:
			ASSERT( false, "unknown scroll type: " + std::to_string( m_scroll_type ) );
	}
}

}
}
