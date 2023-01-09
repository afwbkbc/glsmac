#include "UIObject.h"

#include "engine/Engine.h"
#include "../UI.h"

namespace ui {
namespace object {

UIObject::UIObject() {
	m_position.left = 0;
	m_position.right = 0;
	m_position.top = 0;
	m_position.bottom = 0;
}

void UIObject::Create() {
	m_created = true;
}

void UIObject::Destroy() {
	m_created = false;
}

void UIObject::Align() {
}

void UIObject::Draw() {
}

UIObject *UIObject::GetParentObject() const {
	return m_parent_object;
}

void UIObject::SetParentObject( UIContainer *parent_object ) {
	if ( m_parent_object != NULL )
		throw UIError( "UIObject parent object changing" );
	m_parent_object = parent_object;
	UpdateObjectArea();
}

void UIObject::SetZIndex( float z_index ) {
	if ( z_index < -1.0 || z_index > 1.0 )
		throw UIError( "invalid z-index " + std::to_string( z_index ) );
	//Log( "Changing z-index to " + std::to_string( z_index ) );
	for ( auto it = m_actors.begin() ; it < m_actors.end() ; ++it )
		(*it)->SetPositionZ( z_index );
	m_z_index = z_index;
}

scene::Scene *UIObject::GetSceneOfActor( const scene::actor::Actor *actor ) const {
	scene::Scene *scene;
	switch ( actor->GetType() ) {
		case ( scene::actor::Actor::TYPE_MESH ):
			scene = g_engine->GetUI()->GetShapeScene();
			break;
		case ( scene::actor::Actor::TYPE_TEXT ):
			scene = g_engine->GetUI()->GetTextScene();
			break;
		default:
			throw UIError( "unsupported actor type" );
	}
	return scene;
}

void UIObject::CreateActors() {
	for ( auto it = m_actors.begin() ; it < m_actors.end() ; ++it )
		GetSceneOfActor( *it )->AddActor( *it );
}

void UIObject::DestroyActors() {
	for ( auto it = m_actors.begin() ; it < m_actors.end() ; ++it )
		GetSceneOfActor( *it )->RemoveActor( *it );
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
	if ( m_created )
		Draw();
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

void UIObject::UpdateObjectArea() {
	if ( m_parent_object != NULL ) {
		m_object_area.left = m_parent_object->m_object_area.left + m_position.left;
		m_object_area.right = m_parent_object->m_object_area.right - m_position.right;
		m_object_area.top = m_parent_object->m_object_area.top + m_position.top;
		m_object_area.bottom = m_parent_object->m_object_area.bottom - m_position.bottom;
		if ( m_stick_bits & STICK_WIDTH ) {
			if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
				coord_t parent_center = ( m_parent_object->m_object_area.left + m_parent_object->m_object_area.right ) / 2;
				m_object_area.left = parent_center - m_size.width / 2;
				m_object_area.right = parent_center + m_size.width / 2;
			}
			else if ( m_align & ALIGN_LEFT )
				m_object_area.right = m_object_area.left + m_size.width;
			else if ( m_align & ALIGN_RIGHT )
				m_object_area.left = m_object_area.right - m_size.width;
		}
		if ( m_stick_bits & STICK_HEIGHT ) {
			if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
				coord_t parent_center = ( m_parent_object->m_object_area.top + m_parent_object->m_object_area.bottom ) / 2;
				m_object_area.top = parent_center - m_size.height / 2;
				m_object_area.bottom = parent_center + m_size.height / 2;
			}
			else if ( m_align & ALIGN_TOP )
				m_object_area.bottom = m_object_area.top + m_size.height;
			else if ( m_align & ALIGN_BOTTOM )
				m_object_area.top = m_object_area.bottom - m_size.height;
		}
		if ( m_object_area.left > m_object_area.right )
			m_object_area.left = m_object_area.right;
		if ( m_object_area.top > m_object_area.bottom )
			m_object_area.top = m_object_area.bottom;
		if ( m_parent_object->GetOverflow() == OVERFLOW_HIDDEN ) {
			if ( m_object_area.left < m_parent_object->m_object_area.left )
				m_object_area.left = m_parent_object->m_object_area.left;
			if ( m_object_area.right > m_parent_object->m_object_area.right )
				m_object_area.right = m_parent_object->m_object_area.right;
			if ( m_object_area.top < m_parent_object->m_object_area.top )
				m_object_area.top = m_parent_object->m_object_area.top;
			if ( m_object_area.bottom > m_parent_object->m_object_area.bottom )
				m_object_area.bottom = m_parent_object->m_object_area.bottom;
		}
		m_object_area.width = m_object_area.right - m_object_area.left;
		m_object_area.height = m_object_area.bottom - m_object_area.top;
	}
	else {
		m_object_area.left = 0;
		m_object_area.right = g_engine->GetRenderer()->GetWindowWidth();
		m_object_area.top = 0;
		m_object_area.bottom = g_engine->GetRenderer()->GetWindowHeight();
		m_object_area.width = m_object_area.right;
		m_object_area.height = m_object_area.bottom;
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

} /* namespace object */
} /* namespace ui */
