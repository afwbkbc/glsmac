#include <algorithm>

#include "UIContainer.h"

#include "../UI.h"

using namespace std;

namespace ui {

using namespace event;

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

void UIContainer::Iterate() {
	UIObject::Iterate();
	
	for (auto& c : m_child_objects) {
		c->Iterate();
	}
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
	auto it = find( m_child_objects.begin(), m_child_objects.end(), object );
	if ( it < m_child_objects.end() )
		throw UIError( "duplicate UIObject insertion" );
	m_child_objects.push_back( object );
	object->SetParentObject( this );
	if ( m_created ) {
		CreateChild( object );
	}
}

void UIContainer::RemoveChild( UIObject *object ) {
	auto it = find( m_child_objects.begin(), m_child_objects.end(), object );
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
	return m_z_index - 0.00001; // every child should be a bit above it's parent to be able to be seen
}

void UIContainer::SetOverflow( const overflow_t overflow ) {
	m_overflow = overflow;

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->Realign();
}

void UIContainer::SendEvent( const UIEvent* event ) {
	if (( event->m_flags & UIEvent::EF_MOUSE ) == UIEvent::EF_MOUSE ) {
		for (auto& c : m_child_objects) {
			if (
				( event->m_type == UIEvent::EV_MOUSEMOVE ) || // mousemove needs to be send to all objects for mouseout events to work
				c->IsPointInside( event->m_data.mouse.x, event->m_data.mouse.y ) // other events - only to those actually under mouse pointer
			) {
				auto *child_event = new UIEvent( *event );
				c->SendEvent( child_event );
			}
		}
	}
	
	if (( event->m_flags & UIEvent::EF_KEYBOARD ) == UIEvent::EF_KEYBOARD ) {
		// TODO: send only to focused/active element
		for (auto& c : m_child_objects) {
			auto *child_event = new UIEvent( *event );
			c->SendEvent( child_event );
		}
	}
	
	UIObject::SendEvent( event );
}



} /* namespace object */
} /* namespace ui */
