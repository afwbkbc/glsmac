#include "UIObject.h"

#include <algorithm>

#include "engine/Engine.h"
#include "../UI.h"

using namespace std;

using namespace scene;
namespace scene {
using namespace actor;
}

namespace ui {

using namespace event;

namespace object {

UIObject::UIObject( const string& class_name ) {
	m_position.left = 0;
	m_position.right = 0;
	m_position.top = 0;
	m_position.bottom = 0;
	m_size.aspect_ratio = 0;
	m_size.force_aspect_ratio = false;
	if ( !class_name.empty() ) {
		SetClass( class_name );
	}
}

UIObject::~UIObject() {
	if ( m_created ) {
		Destroy();
	}
}

void UIObject::Create() {
	
	DEBUG_STAT_INC( ui_elements_created );
	DEBUG_STAT_INC( ui_elements_active );
	
	m_created = true;
}

void UIObject::Destroy() {
#if DEBUG
	if ( !m_actors.empty() ) {
		throw UIObjectError( "some actors still present on destruction" );
	}
#endif
	
	HideDebugFrame();
	
	m_created = false;

	DEBUG_STAT_INC( ui_elements_destroyed );
	DEBUG_STAT_DEC( ui_elements_active );
}

void UIObject::Iterate() {
	
}

void UIObject::Align() {
	
}

void UIObject::Draw() {
	ApplyStyleIfNeeded();
}

UIObject *UIObject::GetParentObject() const {
	return m_parent_object;
}

void UIObject::SetParentObject( UIContainer *parent_object ) {
#if DEBUG
	if ( m_parent_object != NULL )
		throw UIError( "parent object already set" );
#endif
	m_parent_object = parent_object;
	m_depth = m_parent_object->m_depth + 1;
	UpdateZIndex();
	UpdateObjectArea();
}

void UIObject::SetZIndex( float z_index ) {
#if DEBUG
		if ( z_index < 0.1 || z_index > 0.9 ) {
			throw UIError( "invalid z-index " + to_string( z_index ) );
		}
#endif
	if ( m_z_index != z_index ) {
		m_z_index = z_index;
		UpdateZIndex();
	}
}

void UIObject::UpdateZIndex() {
	float new_absolute_z_index = std::pow( 0.1, m_depth ) * m_z_index;
	if ( m_parent_object ) {
		new_absolute_z_index += m_parent_object->m_absolute_z_index;
	}
	if ( new_absolute_z_index != m_absolute_z_index ) {
		m_absolute_z_index = new_absolute_z_index;
		for (auto& actor : m_actors) {
			actor->SetPositionZ( m_absolute_z_index );
		}
		Realign();
	}
}

scene::Scene *UIObject::GetSceneOfActor( const Actor *actor ) const {
	scene::Scene *scene;
	switch ( actor->GetType() ) {
		case ( Actor::TYPE_MESH ):
			scene = g_engine->GetUI()->GetShapeScene();
			break;
		case ( Actor::TYPE_TEXT ):
			scene = g_engine->GetUI()->GetTextScene();
			break;
		default:
			throw UIError( "unsupported actor type" );
	}
	return scene;
}

void UIObject::ApplyStyle() {
#if DEBUG
	CheckStylePtr();
#endif
	Log( "Applying style class '" + m_style_class + "' (modifiers: " + to_string( m_style_modifiers ) + ")" );
	
	if ( Has( Style::A_ALIGN ) ) {
		SetAlign( Get( Style::A_ALIGN ) );
	}
	if ( Has( Style::A_LEFT ) ) {
		SetLeft( Get( Style::A_LEFT ) );
	}
	if ( Has( Style::A_TOP ) ) {
		SetTop( Get( Style::A_TOP ) );
	}
	if ( Has( Style::A_RIGHT ) ) {
		SetRight( Get( Style::A_RIGHT ) );
	}
	if ( Has( Style::A_BOTTOM ) ) {
		SetBottom( Get( Style::A_BOTTOM ) );
	}
	if ( Has( Style::A_WIDTH ) ) {
		SetWidth( Get( Style::A_WIDTH ) );
	}
	if ( Has( Style::A_HEIGHT ) ) {
		SetHeight( Get( Style::A_HEIGHT ) );
	}

}

bool UIObject::Has( const Style::attribute_type_t attribute_type ) const {
#if DEBUG
	CheckStylePtr();
#endif
	return m_style->Has( attribute_type, m_style_modifiers );
}

const ssize_t UIObject::Get( const Style::attribute_type_t attribute_type ) const {
#if DEBUG
	CheckStylePtr();
#endif
	return m_style->Get( attribute_type, m_style_modifiers );
}

const Color UIObject::GetColor( const Style::attribute_type_t attribute_type ) const {
#if DEBUG
	CheckStylePtr();
#endif
	return m_style->GetColor( attribute_type, m_style_modifiers );
}

const void* UIObject::GetObject( const Style::attribute_type_t attribute_type ) const {
#if DEBUG
	CheckStylePtr();
#endif
	return m_style->GetObject( attribute_type, m_style_modifiers );
}

#if DEBUG
void UIObject::CheckStylePtr() const {
	if ( !m_style ) {
		throw UIObjectError( "style not set" );
	}
}
#endif

void UIObject::AddActor( Actor* actor ) {
#if DEBUG
	if ( m_actors.find( actor ) != m_actors.end() ) {
		throw UIObjectError( "duplicate actor add" );
	}
#endif
	actor->SetPositionZ( m_absolute_z_index );
	m_actors.insert( actor );
}

void UIObject::RemoveActor( Actor* actor ) {
#if DEBUG
	if ( m_actors.find( actor ) == m_actors.end() ) {
		throw UIObjectError( "actor to be removed not found" );
	}
#endif
	m_actors.erase( actor );
}

void UIObject::EnableActors() {
	for ( auto& actor : m_actors ) {
		GetSceneOfActor( actor )->AddActor( actor );
	}
}

void UIObject::DisableActors() {
	for ( auto& actor : m_actors ) {
		GetSceneOfActor( actor )->RemoveActor( actor );
	}
}

const UIObject::coord_t UIObject::ClampX( const coord_t value ) {
	return g_engine->GetUI()->ClampX( value );
}

const UIObject::coord_t UIObject::ClampY( const coord_t value ) {
	return g_engine->GetUI()->ClampY( value );
}

const UIObject::vertex_t UIObject::ClampXY( const vertex_t value ) {
	return vertex_t( ClampX( value.x ), ClampY( value.y ) );
}

void UIObject::Realign() {
	UpdateObjectArea();
	if ( m_created )
		Align();
}

void UIObject::Redraw() {
	if ( m_created ) {
		Draw();
	}
}

size_t UIObject::GetWidth() const {
	return m_size.width;
}

size_t UIObject::GetHeight() const {
	return m_size.height;
}

void UIObject::SetLeft( const coord_t px ) {
	m_position.left = px;
	m_stick_bits |= STICK_LEFT;
	if ( ! m_align & ALIGN_HCENTER )
		m_align = ( m_align & (~ALIGN_RIGHT) ) | ALIGN_LEFT;
	if ( m_stick_bits & STICK_RIGHT )
		m_stick_bits &= ~( STICK_RIGHT | STICK_WIDTH );
	Realign();

}

void UIObject::SetRight( const coord_t px ) {
	m_position.right = px;
	m_stick_bits |= STICK_RIGHT;
	if ( ! m_align & ALIGN_HCENTER )
		m_align = ( m_align & (~ALIGN_LEFT) ) | ALIGN_RIGHT;
	if ( m_stick_bits & STICK_LEFT )
		m_stick_bits &= ~( STICK_LEFT | STICK_WIDTH );
	Realign();
}

void UIObject::SetTop( const coord_t px ) {
	m_position.top = px;
	m_stick_bits |= STICK_TOP;
	if ( m_stick_bits & STICK_BOTTOM )
		m_stick_bits &= ~( STICK_BOTTOM | STICK_HEIGHT );
	Realign();
}

void UIObject::SetBottom( const coord_t px ) {
	m_position.bottom = px;
	m_stick_bits |= STICK_BOTTOM;
	if ( m_stick_bits & STICK_TOP )
		m_stick_bits &= ~( STICK_TOP | STICK_HEIGHT );
	Realign();
}

void UIObject::SetPadding( const coord_t px ) {
	SetLeft( px );
	SetTop( px );
	SetRight( px );
	SetBottom( px );
}

void UIObject::Maximize() {
	SetPadding( 0 );
}

void UIObject::SetWidth( const coord_t px ) {
	m_size.width = px;
	m_stick_bits |= STICK_WIDTH;
	Realign();
}

void UIObject::SetHeight( const coord_t px ) {
	m_size.height = px;
	m_stick_bits |= STICK_HEIGHT;
	Realign();
}

void UIObject::ForceAspectRatio( const float aspect_ratio ) {
	m_size.aspect_ratio = aspect_ratio;
	m_size.force_aspect_ratio = true;
	Realign();
}

void UIObject::UpdateObjectArea() {
	object_area_t object_area;
	if ( m_parent_object != NULL ) {
		object_area.left = m_parent_object->m_object_area.left + m_position.left;
		object_area.right = m_parent_object->m_object_area.right - m_position.right;
		object_area.top = m_parent_object->m_object_area.top + m_position.top;
		object_area.bottom = m_parent_object->m_object_area.bottom - m_position.bottom;
		if ( m_stick_bits & STICK_WIDTH ) {
			if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
				coord_t parent_center = ( m_parent_object->m_object_area.left + m_parent_object->m_object_area.right ) / 2;
				object_area.left = parent_center - m_size.width / 2;
				object_area.right = parent_center + m_size.width / 2;
			}
			else if ( m_align & ALIGN_LEFT )
				object_area.right = object_area.left + m_size.width;
			else if ( m_align & ALIGN_RIGHT )
				object_area.left = object_area.right - m_size.width;
		}
		if ( m_stick_bits & STICK_HEIGHT ) {
			if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
				coord_t parent_center = ( m_parent_object->m_object_area.top + m_parent_object->m_object_area.bottom ) / 2;
				object_area.top = parent_center - m_size.height / 2;
				object_area.bottom = parent_center + m_size.height / 2;
			}
			else if ( m_align & ALIGN_TOP )
				object_area.bottom = object_area.top + m_size.height;
			else if ( m_align & ALIGN_BOTTOM )
				object_area.top = object_area.bottom - m_size.height;
		}
		if ( object_area.left > object_area.right )
			object_area.left = object_area.right;
		if ( object_area.top > object_area.bottom )
			object_area.top = object_area.bottom;
		if ( m_parent_object->GetOverflow() == OVERFLOW_HIDDEN ) {
			if ( object_area.left < m_parent_object->m_object_area.left )
				object_area.left = m_parent_object->m_object_area.left;
			if ( object_area.right > m_parent_object->m_object_area.right )
				object_area.right = m_parent_object->m_object_area.right;
			if ( object_area.top < m_parent_object->m_object_area.top )
				object_area.top = m_parent_object->m_object_area.top;
			if ( object_area.bottom > m_parent_object->m_object_area.bottom )
				object_area.bottom = m_parent_object->m_object_area.bottom;
		}
		object_area.width = object_area.right - object_area.left;
		object_area.height = object_area.bottom - object_area.top;
		
		if (m_size.force_aspect_ratio) {
			float current_aspect_ratio = (float) object_area.height / object_area.width;
			if (current_aspect_ratio > m_size.aspect_ratio) {
				float new_height = object_area.height * m_size.aspect_ratio / current_aspect_ratio;
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
			else if (current_aspect_ratio < m_size.aspect_ratio) {
				float new_width = object_area.width * current_aspect_ratio / m_size.aspect_ratio;
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
		}
	}
	else {
		object_area.left = 0;
		object_area.right = g_engine->GetRenderer()->GetWindowWidth();
		object_area.top = 0;
		object_area.bottom = g_engine->GetRenderer()->GetWindowHeight();
		object_area.width = object_area.right;
		object_area.height = object_area.bottom;
	}
	
	if (object_area != m_object_area) {
		m_object_area = object_area;
		
		if (m_created) {
			// process any mouseover/mouseout events
			// mouse may not being moved, but if object area has changed - they should be able to fire too
			g_engine->GetUI()->SendMouseMoveEvent( this );
			
#if DEBUG
			// resize debug frame to match new area
			if ( m_has_debug_frame ) {
				g_engine->GetUI()->ResizeDebugFrame( this );
			}
#endif
		}
	}
	
}

void UIObject::SetAlign( const alignment_t align ) {
	m_align = align;
	if ( ! ( m_align & ALIGN_HCENTER ) )
		m_align |= ALIGN_LEFT;
	if ( ! ( m_align & ALIGN_VCENTER ) )
		m_align |= ALIGN_TOP;
}

void UIObject::SetHAlign( const alignment_t align ) {
	m_align = ( m_align & ALIGN_VCENTER ) | ( align & ALIGN_HCENTER );
}

void UIObject::SetVAlign( const alignment_t align ) {
	m_align = ( m_align & ALIGN_HCENTER ) | ( align & ALIGN_VCENTER );
}

void UIObject::SendEvent( const UIEvent* event ) {
	switch ( event->m_type ) {
		case UIEvent::EV_MOUSEMOVE: {
			if ( IsPointInside( event->m_data.mouse.x, event->m_data.mouse.y ) ) {
				if ( ( m_state & STATE_MOUSEOVER ) != STATE_MOUSEOVER ) {
					m_state |= STATE_MOUSEOVER;
					AddStyleModifier( Style::M_HOVER );
					OnMouseOver( &event->m_data );
				}
			}
			else {
				if ( ( m_state & STATE_MOUSEOVER ) == STATE_MOUSEOVER ) {
					m_state &= ~STATE_MOUSEOVER;
					RemoveStyleModifier( Style::M_HOVER );
					OnMouseOut( &event->m_data );
				}
			}
			break;
		}
		case UIEvent::EV_MOUSEDOWN: {
			OnMouseDown( &event->m_data );
			break;
		}
		case UIEvent::EV_KEYDOWN: {
			OnKeyDown( &event->m_data );
			break;
		}
		case UIEvent::EV_MOUSECLICK: {
			//OnMouseClick( &event->m_data );
		}
	}
	
	// processed, not needed anymore
	DELETE( event );
}

UIObject::vertex_t UIObject::GetAreaPosition() const {
	return { m_object_area.left, m_object_area.top };
}

pair<UIObject::vertex_t, UIObject::vertex_t> UIObject::GetAreaGeometry() const {
	return {
		{ m_object_area.left, m_object_area.top },
		{ m_object_area.right, m_object_area.bottom }
	};
}

bool UIObject::IsPointInside( const size_t x, const size_t y ) const {
	return (
		x >= m_object_area.left &&
		y >= m_object_area.top &&
		x <= m_object_area.right &&
		y <= m_object_area.bottom
	);
}

void UIObject::SetClass( const string& style_class ) {
#if DEBUG
	if ( !m_style_class.empty() ) {
		throw UIObjectError( "style class already set to '" + m_style_class + "'" );
	}
	if ( m_style_loaded ) {
		throw UIObjectError( "style '" + m_style_class + "' already loaded" );
	}
#endif
	Log("Setting style class '" + style_class + "'");
	m_style_class = style_class;
}

#if DEBUG
void UIObject::ShowDebugFrame() {
	if ( !m_has_debug_frame ) {
		g_engine->GetUI()->ShowDebugFrame( this );
		m_has_debug_frame = true;
	}
}

void UIObject::HideDebugFrame() {
	if ( m_has_debug_frame ) {
		g_engine->GetUI()->HideDebugFrame( this );
		m_has_debug_frame = false;
	}
}
#endif

void UIObject::ApplyStyleIfNeeded() {
	if ( !m_style_loaded ) {
		if ( !m_style_class.empty() ) {
			m_style = g_engine->GetUI()->GetStyle( m_style_class );
			ApplyStyle();
		}
		m_style_loaded = true;
	}
}

void UIObject::AddStyleModifier( const Style::modifier_t modifier ) {
#if DEBUG
	if ( m_style_modifiers & modifier ) {
		throw UIObjectError( "style modifier " + to_string( modifier ) + " already added" );
	}
#endif
	m_style_modifiers |= modifier;
	if ( m_style ) {
		ApplyStyle();
	}
}

void UIObject::RemoveStyleModifier( const Style::modifier_t modifier ) {
#if DEBUG
	if ( !(m_style_modifiers & modifier) ) {
		throw UIObjectError( "style modifier " + to_string( modifier ) + " already removed" );
	}
#endif
	m_style_modifiers &= ~modifier;
	if ( m_style ) {
		ApplyStyle();
	}
}


} /* namespace object */
} /* namespace ui */
