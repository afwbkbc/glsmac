#include "Geometry.h"

#include <cmath>

#include "common/Assert.h"

#include "engine/Engine.h"
#include "graphics/Graphics.h"

namespace ui {
namespace geometry {

Geometry::Geometry( const UI* const ui, Geometry* const parent, const geometry_type_t type )
	: m_ui( ui )
	, m_parent( parent )
	, m_type( type ) {
	UpdateArea();
	if ( m_parent ) {
		m_parent->m_children.insert( this );
		m_parent->UpdateEffectiveArea();
	}
}

Geometry::~Geometry() {
	if ( m_parent ) {
		m_parent->m_children.erase( this );
	}
	for ( const auto& child : m_children ) {
		child->Detach();
	}
}

Rectangle* Geometry::AsRectangle() const {
	ASSERT( m_type == GT_RECTANGLE, "invalid geometry type" );
	return (Rectangle*)this;
}

Text* Geometry::AsText() const {
	ASSERT( m_type == GT_TEXT, "invalid geometry type" );
	return (Text*)this;
}

void Geometry::SetParent( Geometry* const other ) {
	ASSERT( other, "other geometry is null" );
	if ( m_parent ) {
		m_parent->m_children.erase( this );
		m_parent->UpdateEffectiveArea();
	}
	m_parent = other;
	m_parent->m_children.insert( this );
	m_parent->UpdateEffectiveArea();
}

void Geometry::SetLeft( const coord_t px ) {
	m_left = px;
	m_stick_bits |= STICK_LEFT;
	if ( m_stick_bits & STICK_RIGHT ) {
		m_stick_bits &= ~( STICK_RIGHT | STICK_WIDTH );
	}
	NeedUpdate();
}

const coord_t Geometry::GetLeft() const {
	return m_left;
}

void Geometry::SetTop( const coord_t px ) {
	m_top = px;
	m_stick_bits |= STICK_TOP;
	if ( m_stick_bits & STICK_BOTTOM ) {
		m_stick_bits &= ~( STICK_BOTTOM | STICK_HEIGHT );
	}
	NeedUpdate();
}

const coord_t Geometry::GetTop() const {
	return m_top;
}

void Geometry::SetRight( const coord_t px ) {
	m_right = px;
	m_stick_bits |= STICK_RIGHT;
	if ( m_stick_bits & STICK_LEFT ) {
		m_stick_bits &= ~( STICK_LEFT | STICK_WIDTH );
	}
	NeedUpdate();
}

void Geometry::SetBottom( const coord_t px ) {
	m_bottom = px;
	m_stick_bits |= STICK_BOTTOM;
	if ( m_stick_bits & STICK_TOP ) {
		m_stick_bits &= ~( STICK_TOP | STICK_HEIGHT );
	}
	NeedUpdate();
}

void Geometry::SetWidth( const coord_t px ) {
	m_width = px;
	m_stick_bits |= STICK_WIDTH;
	NeedUpdate();
}

void Geometry::SetHeight( const coord_t px ) {
	m_height = px;
	m_stick_bits |= STICK_HEIGHT;
	NeedUpdate();
}

void Geometry::SetPadding( const coord_t px ) {
	if ( m_padding != px ) {
		m_padding = px;
		NeedUpdate();
	}
}

void Geometry::SetAlign( const align_t align ) {
	if ( align != m_align ) {
		m_align = align;
		if ( !( m_align & ALIGN_HCENTER ) ) {
			m_align |= ALIGN_LEFT;
		}
		if ( !( m_align & ALIGN_VCENTER ) ) {
			m_align |= ALIGN_TOP;
		}
		NeedUpdate();
	}
}

void Geometry::SetPosition( const position_t position ) {
	if ( m_position != position ) {
		m_position = position;
		NeedUpdate();
	}
}

void Geometry::SetZIndex( const coord_t zindex ) {
	if ( m_zindex != zindex ) {
		m_zindex = zindex;
		NeedUpdate();
	}
}

void Geometry::SetOverflowMode( const overflow_mode_t mode ) {
	if ( mode != m_overflow_mode ) {
		m_overflow_mode = mode;
		NeedUpdate();
	}
}

void Geometry::NeedUpdate() {
	// for now update immediately, later queue and execute it once after
	Update();
}

const Geometry::area_t& Geometry::GetEffectiveArea() const {
	return m_effective_area;
}

void Geometry::OnChildUpdate() {
	if ( m_overflow_mode == OM_RESIZE ) {
		// resize area from children ( TODO: optimize? )
		m_area = {};
		for ( const auto& child : m_children ) {
			if ( child->m_is_visible ) {
				m_area.EnlargeTo( child->m_area );
			}
		}
		FixArea( m_area );
		NeedUpdate();
	}
	UpdateEffectiveArea();
	RunHandlers( GH_ON_CHILD_UPDATE );
	if ( m_parent ) {
		m_parent->OnChildUpdate();
	}
}

const geometry_handler_id_t Geometry::AddHandler( const geometry_handler_type_t type, const std::function< void() >& f ) {
	if ( m_next_handler_id >= 255 ) {
		THROW( "bug: too many handlers" );
	}
	m_next_handler_id++;
	ASSERT( m_handlers[ type ].find( m_next_handler_id ) == m_handlers[ type ].end(), "handler id already exists" );
	ASSERT( m_handler_types.find( m_next_handler_id ) == m_handler_types.end(), "handler id already exists" );
	m_handlers[ type ].insert(
		{
			m_next_handler_id,
			f
		}
	);
	m_handler_types.insert(
		{
			m_next_handler_id,
			type
		}
	);
	return m_next_handler_id;
}

void Geometry::RemoveHandler( const geometry_handler_id_t id ) {
	ASSERT( m_handler_types.find( id ) != m_handler_types.end(), "handler id not found" );
	const auto type = m_handler_types.at( id );
	ASSERT( m_handlers.find( type ) != m_handlers.end(), "handler type not found" );
	ASSERT( m_handlers.at( type ).find( id ) != m_handlers.at( type ).end(), "handler id not found" );
	m_handlers.at( type ).erase( id );
	m_handler_types.erase( id );
}

const bool Geometry::Contains( const types::Vec2< ssize_t >& position ) const {
	return
		m_is_visible &&
			position.x > m_effective_area.left &&
			position.y > m_effective_area.top &&
			position.x <= ( m_effective_area.left + m_effective_area.width + 1 ) &&
			position.y <= ( m_effective_area.top + m_effective_area.height + 1 );
}

void Geometry::Show() {
	if ( !m_is_visible ) {
		m_is_visible = true;
		NeedUpdate();
	}
}

void Geometry::Hide() {
	if ( m_is_visible ) {
		m_is_visible = false;
		NeedUpdate();
	}
}

void Geometry::Detach() {
	ASSERT( m_parent, "parent not set" );
	m_parent = nullptr;
}

void Geometry::Update() {
	UpdateArea();
	UpdateImpl();
	for ( const auto& geometry : m_children ) {
		geometry->Update();
	}
	UpdateEffectiveArea();
}

void Geometry::UpdateArea() {
	//Log( "Stick bits = " + std::to_string( m_stick_bits ) );
	area_t object_area = {};
	object_area.zindex = m_zindex;

	if ( m_parent ) {
		area_t area;
		switch ( m_position ) {
			case POSITION_RELATIVE: {
				area = m_parent->m_area;
				break;
			}
			case POSITION_ABSOLUTE: {
				const auto& g = g_engine->GetGraphics();
				area.left = 0;
				area.top = 0;
				area.right = area.width = g->GetViewportWidth() - 1;
				area.bottom = area.height = g->GetViewportHeight() - 1;
				break;
			}
			default:
				THROW( "unknown position: " + std::to_string( m_position ) );
		}
		object_area.left = area.left + m_padding + m_left;
		object_area.right = area.right - m_padding - m_right;
		object_area.top = area.top + m_padding + m_top;
		object_area.bottom = area.bottom - m_padding - m_bottom;
		if ( m_stick_bits & STICK_WIDTH ) {
			if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
				coord_t parent_center = ( area.left + area.right ) / 2;
				object_area.left = parent_center - m_width / 2;
				object_area.right = parent_center + m_width / 2;
			}
			else if ( m_align & ALIGN_LEFT ) {
				object_area.right = object_area.left + m_width;
			}
			else if ( m_align & ALIGN_RIGHT ) {
				object_area.left = object_area.right - m_width;
			}
		}
		if ( m_stick_bits & STICK_HEIGHT ) {
			if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
				coord_t parent_center = ( area.top + area.bottom ) / 2;
				object_area.top = parent_center - m_height / 2;
				object_area.bottom = parent_center + m_height / 2;
			}
			else if ( m_align & ALIGN_TOP ) {
				object_area.bottom = object_area.top + m_height;
			}
			else if ( m_align & ALIGN_BOTTOM ) {
				object_area.top = object_area.bottom - m_height;
			}
		}
		if ( object_area.left > object_area.right ) {
			object_area.left = object_area.right;
		}
		if ( object_area.top > object_area.bottom ) {
			object_area.top = object_area.bottom;
		}

		object_area.width = object_area.right - object_area.left;
		object_area.height = object_area.bottom - object_area.top;

		// TODO
		/*if ( m_force_aspect_ratio ) {
			float current_aspect_ratio = (float)object_area.height / object_area.width;
			if ( current_aspect_ratio > m_aspect_ratio ) {
				float new_height = object_area.height * m_aspect_ratio / current_aspect_ratio;
				if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
					object_area.top += ( object_area.height - new_height ) / 2;
					object_area.bottom -= ( object_area.height - new_height ) / 2;
				}
				else if ( m_align & ALIGN_TOP ) {
					object_area.bottom -= ( object_area.height - new_height );
				}
				else if ( m_align & ALIGN_BOTTOM ) {
					object_area.top += ( object_area.height - new_height );
				}
				object_area.height = new_height;
			}
			else if ( current_aspect_ratio < m_aspect_ratio ) {
				float new_width = object_area.width * current_aspect_ratio / m_aspect_ratio;
				if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
					object_area.left += ( object_area.width - new_width ) / 2;
					object_area.right -= ( object_area.width - new_width ) / 2;
				}
				else if ( m_align & ALIGN_LEFT ) {
					object_area.right -= ( object_area.width - new_width );
				}
				else if ( m_align & ALIGN_RIGHT ) {
					object_area.left += ( object_area.width - new_width );
				}
				object_area.width = new_width;
			}
		}*/
	}
	else {
		// only for Root object, it always fills entire screen
		object_area.left = 0;
		object_area.top = 0;
		object_area.right = m_width;
		object_area.bottom = m_height;
		object_area.width = m_width;
		object_area.height = m_height;
	}

	// TODO
	/*if ( m_margin ) {
		if ( m_margin.left ) {
			object_area.left += m_margin.left;
		}
		if ( m_margin.top ) {
			object_area.top += m_margin.top;
		}
		if ( m_margin.right ) {
			object_area.right -= m_margin.right;
		}
		if ( m_margin.bottom ) {
			object_area.bottom -= m_margin.bottom;
		}
		if ( object_area.right < object_area.left ) {
			object_area.right = object_area.left;
		}
		if ( object_area.bottom < object_area.top ) {
			object_area.bottom = object_area.top;
		}
		object_area.width = object_area.right - object_area.left;
		object_area.height = object_area.bottom - object_area.top;
	}*/

	if ( object_area != m_area ) {

		m_area = object_area;

		RunHandlers( GH_ON_AREA_UPDATE );

/*		if ( m_created ) {
			// process any mouseover/mouseout events
			// mouse may not being moved, but if object area has changed - they should be able to fire too
			// don't do this if parent captures MouseOver event tho, we're not supposed to receive mouseovers in that case
			if ( !IsEventContextOverridden( EC_MOUSEMOVE ) ) {
				g_engine->GetUI()->SendMouseMoveEvent( this );
			}

		}
#ifdef DEBUG
		// resize debug frame to match new area
		if ( m_has_debug_frame ) {
			g_engine->GetUI()->ResizeDebugFrame( this );
		}
#endif*/
	}
}

void Geometry::UpdateEffectiveArea() {
	area_t effective_area = {};
	if ( m_is_visible ) {
		effective_area = m_area;
		if ( m_overflow_mode != OM_HIDDEN ) {
			for ( const auto& child : m_children ) {
				if ( child->m_is_visible ) {
					effective_area.EnlargeTo( child->GetEffectiveArea() );
				}
			}
			FixArea( effective_area );
		}
	}

	if ( effective_area != m_effective_area ) {
		const bool should_resize = std::round( effective_area.width ) != std::round( m_effective_area.width ) || std::round( effective_area.height ) != std::round( m_effective_area.height );
		m_effective_area = effective_area;
		if ( should_resize && m_on_resize ) {
			m_on_resize( m_effective_area.width, m_effective_area.height );
		}
		if ( m_parent ) {
			m_parent->OnChildUpdate();
		}
		RunHandlers( GH_ON_EFFECTIVE_AREA_UPDATE );
	}
}

void Geometry::FixArea( area_t& area ) {
	const auto& g = g_engine->GetGraphics();
	const auto maxx = g->GetViewportWidth() - 1;
	const auto maxy = g->GetViewportHeight() - 1;
	if ( area.left < 0 ) {
		area.left = 0;
	}
	if ( area.right > maxx ) {
		area.right = maxx;
	}
	if ( area.top < 0 ) {
		area.top = 0;
	}
	if ( area.bottom > maxy ) {
		area.bottom = maxy;
	}
}

void Geometry::RunHandlers( const geometry_handler_type_t type ) const {
	const auto& it = m_handlers.find( type );
	if ( it != m_handlers.end() ) {
		for ( const auto& it2 : it->second ) {
			it2.second();
		}
	}
}

}
}
