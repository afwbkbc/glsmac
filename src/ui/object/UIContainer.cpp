#include <algorithm>

#include "UIContainer.h"

#include "../UI.h"

namespace ui {

using namespace event;

namespace object {

UIContainer::UIContainer( const std::string& class_name )
	: UIObject( class_name )
{
	//
}

void UIContainer::Create() {
	UIObject::Create();

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		CreateChild( *it );
}

void UIContainer::Destroy() {
	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		DestroyChild( *it );
	
	ASSERT( m_child_objects.empty(), "some children still alive upon parent destruction!" );
	
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
	object->AddStyleModifier( m_style_modifiers );
	object->Realign();
	object->EnableActors();
	object->Redraw();
}

void UIContainer::DestroyChild( UIObject *object ) {
	if ( m_created ) {
		object->DisableActors();
		object->Destroy();
	}
	DELETE( object );
}

void UIContainer::AddChild( UIObject *object ) {
	ASSERT( !object->GetParentObject(),"non-free UIObject insertion" );
	ASSERT( find( m_child_objects.begin(), m_child_objects.end(), object ) == m_child_objects.end(), "duplicate UIObject insertion" );
	//Log( "Adding child " + object->GetName() );
	m_child_objects.push_back( object );
	object->SetParentObject( this );
	object->SetOverriddenEventContexts( m_event_contexts | m_overridden_event_contexts );
	if ( m_created ) {
		CreateChild( object );
	}
}

void UIContainer::RemoveChild( UIObject *object ) {
	auto it = find( m_child_objects.begin(), m_child_objects.end(), object );
	ASSERT( it != m_child_objects.end(), "UIObject to be removed not found" );
	//Log( "Removing child " + object->GetName() );
	m_child_objects.erase( it, it + 1 );
	DestroyChild( object );
}

void UIContainer::UpdateZIndex() {
	UIObject::UpdateZIndex();
	
	for ( auto &child : m_child_objects ){
		child->UpdateZIndex();
	}
}

void UIContainer::Realign() {
	UIObject::Realign();
	
	if ( !m_are_realigns_blocked ) {
		for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
			(*it)->Realign();
	}
}

void UIContainer::Redraw() {
	UIObject::Redraw();
	
	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->Redraw();
}

void UIContainer::SetOverflow( const overflow_t overflow ) {
	m_overflow = overflow;

	for ( auto it = m_child_objects.begin() ; it < m_child_objects.end() ; ++it )
		(*it)->Realign();
}

void UIContainer::ProcessEvent( UIEvent* event ) {
	
	if ( !( event->m_flags & UIEvent::EF_MOUSE ) ) {
		// non-mouse events are processed by parent before children
		UIObject::ProcessEvent( event );
		if ( event->IsProcessed() ) {
			return;
		}
	}
	
	bool is_processed;
	if ( event->m_flags & UIEvent::EF_MOUSE ) {
		for (auto& c : m_child_objects) {
			if (
				( event->m_type == UIEvent::EV_MOUSE_MOVE ) || // mousemove needs to be send to all objects for mouseout events to work
				c->IsPointInside( event->m_data.mouse.absolute.x, event->m_data.mouse.absolute.y ) // other events - only to those actually under mouse pointer
			) {
				NEWV( child_event, UIEvent, event );
				c->ProcessEvent( child_event );
				is_processed = child_event->IsProcessed();
				DELETE( child_event );
				if ( is_processed ) {
					event->SetProcessed();
					return;
				}
			}
		}
	}
	
	if (( event->m_flags & UIEvent::EF_KEYBOARD ) == UIEvent::EF_KEYBOARD ) {
		// TODO: send only to focused/active element
		for (auto& c : m_child_objects) {
			NEWV( child_event, UIEvent, event );
			c->ProcessEvent( child_event );
			is_processed = child_event->IsProcessed();
			DELETE( child_event );
			if ( is_processed ) {
				event->SetProcessed();
				return;
			}
		}
	}

	if ( event->m_flags & UIEvent::EF_MOUSE ) {
		// mouse events are processed by children before parent
		if ( !event->IsProcessed() ) {
			UIObject::ProcessEvent( event );
		}
	}
}

void UIContainer::ApplyStyle() {
	UIObject::ApplyStyle();
	
	for (auto& c : m_child_objects) {
		////c->ReloadStyle();
		c->ApplyStyle(); // ???
	}
}

void UIContainer::ReloadStyle() {
	if ( m_style_loaded ) {
		UIObject::ReloadStyle();
		
		for (auto& c : m_child_objects) {
			c->ReloadStyle();
		}
	}
}

void UIContainer::SetEventContexts( event_context_t contexts ) {
	UIObject::SetEventContexts( contexts );
	
	for (auto& c : m_child_objects) {
		c->SetOverriddenEventContexts( contexts );
	}
}

void UIContainer::AddEventContexts( event_context_t contexts ) {
	UIObject::AddEventContexts( contexts );
	
	for (auto& c : m_child_objects) {
		c->AddOverriddenEventContexts( contexts );
	}
}

void UIContainer::SetOverriddenEventContexts( event_context_t contexts ) {
	UIObject::SetOverriddenEventContexts( contexts );
	
	for (auto& c : m_child_objects) {
		c->SetOverriddenEventContexts( contexts );
	}
}

void UIContainer::AddOverriddenEventContexts( event_context_t contexts ) {
	UIObject::AddOverriddenEventContexts( contexts );
	
	for (auto& c : m_child_objects) {
		c->AddOverriddenEventContexts( contexts );
	}
}

void UIContainer::AddStyleModifier( const Style::modifier_t modifier ) {
	UIObject::AddStyleModifier( modifier );
	
	for (auto& c : m_child_objects) {
		if ( !c->HasStyleModifier( modifier ) ) {
			c->AddStyleModifier( modifier );
		}
	}
}

void UIContainer::RemoveStyleModifier( const Style::modifier_t modifier ) {
	UIObject::RemoveStyleModifier( modifier );
	
	for (auto& c : m_child_objects) {
		if ( c->HasStyleModifier( modifier ) ) {
			c->RemoveStyleModifier( modifier );
		}
	}
}

void UIContainer::BlockEvents() {
	UIObject::BlockEvents();
	
	for (auto& c : m_child_objects) {
		c->BlockEvents();
	}
}

void UIContainer::UnblockEvents() {
	UIObject::UnblockEvents();

	for (auto& c : m_child_objects) {
		c->UnblockEvents();
	}
}

void UIContainer::Show() {
	if ( !m_is_visible ) {
		UIObject::Show();
		for (auto& c : m_child_objects) {
			c->Show();
		}
	}
}

void UIContainer::Hide() {
	if ( m_is_visible ) {
		UIObject::Hide();
		for (auto& c : m_child_objects) {
			c->Hide();
		}
	}
}

const std::string UIContainer::Subclass( const std::string& class_name ) const {
	//if (m_style_class.empty()) {
		return "";
	/*}
	else {
		return m_style_class + class_name;
	}*/
}

} /* namespace object */
} /* namespace ui */
