#include <algorithm>

#include "UIContainer.h"

#include "../UI.h"

namespace ui {
namespace object {

void UIContainer::Create() {
	UIObject::Create();

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		CreateChild( *it );
}

void UIContainer::Destroy() {
	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		DestroyChild( *it );

	UIObject::Destroy();
}

void UIContainer::CreateChild( UIObject *object ) {
	object->Create();
	object->Realign();
	object->CreateActors();
	object->SetZIndex( GetChildZIndex() );
	object->Redraw();
}

void UIContainer::DestroyChild( UIObject *object ) {
	if ( m_created ) {
		object->DestroyActors();
		object->Destroy();
	}
	delete object;
}

void UIContainer::AddChild( UIObject *object ) {
	if ( object->GetParentObject() != NULL )
		throw UIError( "non-free UIObject insertion" );
	auto it = std::find( m_child_objects.begin(), m_child_objects.end(), object );
	if ( it < m_child_objects.end() )
		throw UIError( "duplicate UIObject insertion" );
	m_child_objects.push_back( object );
	object->SetParentObject( this );
	if ( m_created ) {
		CreateChild( object );
	}
}

void UIContainer::RemoveChild( UIObject *object ) {
	auto it = std::find( m_child_objects.begin(), m_child_objects.end(), object );
	if ( it == m_child_objects.end() )
		throw UIError( "UIObject to be removed not found" );
	m_child_objects.erase( it, it + 1 );
	DestroyChild( object );
}

void UIContainer::SetZIndex( float z_index ) {
	UIObject::SetZIndex( z_index );

	float child_z_index = GetChildZIndex();
	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->SetZIndex( child_z_index );
}

void UIContainer::Realign() {
	UIObject::Realign();

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->Realign();
}

void UIContainer::Redraw() {
	UIObject::Redraw();

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->Redraw();
}

const float UIContainer::GetChildZIndex() const {
	return m_z_index - 0.00001;
}

void UIContainer::SetOverflow( const overflow_t overflow ) {
	m_overflow = overflow;

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->Realign();
}

} /* namespace object */
} /* namespace ui */
