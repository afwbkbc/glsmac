#include "Geometry.h"

#include "types/mesh/Rectangle.h"

#include "ui/UI.h"

namespace ui {

Geometry::Geometry( const UI* const ui, Geometry* const parent )
	: m_ui( ui )
	, m_parent( parent ) {
	if ( m_parent ) {
		m_parent->m_children.insert( this );
	}
	Update();
}

Geometry::~Geometry() {
	if ( m_parent ) {
		m_parent->m_children.erase( this );
	}
}

void Geometry::SetMesh( types::mesh::Rectangle* const mesh ) {
	m_mesh = mesh;
	UpdateMesh();
}

void Geometry::SetLeft( const coord_t px ) {
	m_left = px;
	m_stick_bits |= STICK_LEFT;
	if ( m_stick_bits & STICK_RIGHT ) {
		m_stick_bits &= ~( STICK_RIGHT | STICK_WIDTH );
	}
	NeedUpdate();
}

void Geometry::SetTop( const coord_t px ) {
	m_top = px;
	m_stick_bits |= STICK_TOP;
	if ( m_stick_bits & STICK_BOTTOM ) {
		m_stick_bits &= ~( STICK_BOTTOM | STICK_HEIGHT );
	}
	NeedUpdate();
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

void Geometry::SetAlign( const align_t align ) {
	m_align = align;
	if ( !( m_align & ALIGN_HCENTER ) ) {
		m_align |= ALIGN_LEFT;
	}
	if ( !( m_align & ALIGN_VCENTER ) ) {
		m_align |= ALIGN_TOP;
	}
	NeedUpdate();
}

void Geometry::NeedUpdate() {
	// for now update immediately, later queue and execute it once after
	Update();
}

void Geometry::Update() {
	UpdateArea();
	UpdateMesh();
	for ( const auto& geometry : m_children ) {
		geometry->Update();
	}
}

void Geometry::UpdateArea() {
	//Log( "Stick bits = " + std::to_string( m_stick_bits ) );
	area_t object_area = {};
	if ( m_parent != nullptr ) {
		const auto area = m_parent->m_area;
		object_area.left = area.left + m_left;
		object_area.right = area.right - m_right;
		object_area.top = area.top + m_top;
		object_area.bottom = area.bottom - m_bottom;
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

void Geometry::UpdateMesh() {
	if ( m_mesh ) {
		m_mesh->SetCoords(
			m_ui->Clamp(
				{
					m_area.left,
					m_area.top,
				}
			),
			m_ui->Clamp(
				{
					m_area.right,
					m_area.bottom,
				}
			),
			0.5f
		);
	}
}

}
