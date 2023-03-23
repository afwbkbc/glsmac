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

	for ( auto& child : m_child_objects ) {
		CreateChild( child );
	}
}

void UIContainer::Destroy() {
	for ( auto& child : m_child_objects ) {
		DestroyChild( child );
	}
	
	ASSERT( m_child_objects.empty(), "some children still alive upon parent destruction!" );
	
	UIObject::Destroy();
}

void UIContainer::Iterate() {
	UIObject::Iterate();
	
	for ( auto& child : m_child_objects ) {
		child->Iterate();
	}
}

void UIContainer::CreateChild( UIObject *object ) {
	object->ApplyStyleIfNeeded();
	object->Create();
	object->AddStyleModifier( m_style_modifiers );
	//object->Realign();
	object->EnableActors();
	if ( !m_is_actually_visible ) {
		object->HideActors();
	}
	//object->Redraw();

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
	if ( m_overflow == OVERFLOW_GROW ) {
		GrowFromObject( object );
	}
	else if (
		m_overflow == OVERFLOW_HIDDEN
	) {
		Log( "Setting overflow limits for " + object->GetName() );
		object->SetAreaLimitsByObject( this );
	}
	if ( m_area_limits.enabled ) {
		if ( m_area_limits.source_object ) {
			object->SetAreaLimitsByObject( m_area_limits.source_object );
		}
		else {
			object->SetAreaLimits( m_area_limits.limits );
		}
	}
	if ( m_created ) {
		ApplyStyleIfNeeded();
		CreateChild( object );
		Refresh();
	}
}

void UIContainer::RemoveChild( UIObject *object ) {
	auto it = find( m_child_objects.begin(), m_child_objects.end(), object );
	ASSERT( it != m_child_objects.end(), "UIObject to be removed not found" );
	//Log( "Removing child " + object->GetName() );
	m_child_objects.erase( it, it + 1 );
	DestroyChild( object );
	if ( m_overflow == OVERFLOW_GROW ) {
		ShrinkToFit();
	}
	Refresh();
}

void UIContainer::UpdateZIndex() {
	UIObject::UpdateZIndex();
	
	for ( auto &child : m_child_objects ){
		child->UpdateZIndex();
	}
}

void UIContainer::SetPadding( const coord_t px ) {
	if ( m_padding != px ) {
		m_padding = px;
		if ( !m_are_realigns_blocked ) {
			for ( auto& child : m_child_objects ) {
				child->Realign();
			}
		}
	}
}

const UIObject::coord_t UIContainer::GetPadding() const {
	return m_padding;
}

void UIContainer::RealignNow() {
	UIObject::RealignNow();
	
	if ( !m_are_realigns_blocked ) {
		for ( auto& child : m_child_objects ) {
			child->RealignNow();
		}
	}
}

void UIContainer::Redraw() {
	UIObject::Redraw();
	
	for ( auto& child : m_child_objects ) {
		child->Redraw();
	}
}

void UIContainer::SetOverflow( const overflow_t overflow ) {
	m_overflow = overflow;
	
	if ( m_overflow == OVERFLOW_HIDDEN ) {
		for ( auto& child : m_child_objects ) {
			if ( child->GetOverflow() != OVERFLOW_VISIBLE_ALWAYS ) {
				child->SetAreaLimitsByObject( this );
			}
		}
	}
	
	for ( auto& child : m_child_objects ) {
		child->Realign();
	}
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

const UIObject::object_area_t UIContainer::GetInternalObjectArea() {
	auto area = UIObject::GetInternalObjectArea();
	if ( m_padding > 0 ) {
		area.left += m_padding;
		area.top += m_padding;
		area.right -= m_padding;
		area.bottom -= m_padding;
		if ( area.right < area.left ) {
			area.right = area.left;
		}
		if ( area.bottom < area.top ) {
			area.bottom = area.top;
		}
		area.height = area.top - area.bottom;
		area.width = area.right - area.left;
	}
	return area;
}

void UIContainer::ApplyStyle() {
	UIObject::ApplyStyle();
	
	if ( Has( Style::A_PADDING ) ) {
		SetPadding( Get( Style::A_PADDING ) );
	}
	
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

void UIContainer::SetAreaLimits( const coord_box_t limits ) {
	UIObject::SetAreaLimits( limits );
	
	for (auto& c : m_child_objects) {
		c->SetAreaLimits( limits );
	}
}

void UIContainer::SetAreaLimitsByObject( UIObject* source_object ) {
	UIObject::SetAreaLimitsByObject( source_object );

	for (auto& c : m_child_objects) {
		c->SetAreaLimitsByObject( source_object );
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

void UIContainer::ShowActors() {
	if ( m_is_visible && !m_is_actually_visible ) {
		UIObject::ShowActors();
		for (auto& c : m_child_objects) {
			c->ShowActors();
		}
	}
}

void UIContainer::HideActors() {
	if ( m_is_actually_visible ) {
		UIObject::HideActors();
		for (auto& c : m_child_objects) {
			c->HideActors();
		}
	}
}

void UIContainer::GrowFromObjectMaybe( UIObject *object ) {
	if ( m_overflow == OVERFLOW_GROW ) {
		GrowFromObject( object );
	}
}

void UIContainer::ShrinkToFitMaybe() {
	if ( m_overflow == OVERFLOW_GROW ) {
		ShrinkToFit();
	}
}

void UIContainer::UpdateAreaLimits() {
	UIObject::UpdateAreaLimits();
	
	for ( auto& c : m_child_objects ) {
		c->UpdateAreaLimits();
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

void UIContainer::GrowFromObject( UIObject *object ) {
	const auto& margin = object->GetMargin();
	const coord_t need_width = object->GetLeft() + object->GetWidth() + object->GetRight() + margin.left + margin.right;
	if ( need_width > GetWidth() ) {
		//Log( "Growing width from " + std::to_string( GetWidth() ) + " to " + std::to_string( need_width ) );
		SetWidth( need_width );
	}
	const coord_t need_height = object->GetTop() + object->GetHeight() + object->GetBottom() + margin.top + margin.bottom;
	if ( need_height > GetHeight() ) {
		//Log( "Growing height from " + std::to_string( GetHeight() ) + " to " + std::to_string( need_height ) );
		SetHeight( need_height );
	}
}

void UIContainer::ShrinkToFit() {
	Vec2< coord_t > shrink_to = { 0.0f, 0.0f };
	for ( auto& object : m_child_objects ) {
		const auto& margin = object->GetMargin();
		shrink_to.x = std::max< coord_t >( shrink_to.x, object->GetLeft() + object->GetWidth() + object->GetRight() + margin.left + margin.right );
		shrink_to.y = std::max< coord_t >( shrink_to.y, object->GetTop() + object->GetHeight() + object->GetBottom() + margin.top + margin.bottom );
	}
	if ( GetWidth() > shrink_to.x ) {
		//Log( "Shrinking width from " + std::to_string( GetWidth() ) + " to " + std::to_string( shrink_to.x ) );
		SetWidth( shrink_to.x );
	}
	if ( GetHeight() > shrink_to.y ) {
		//Log( "Shrinking height from " + std::to_string( GetHeight() ) + " to " + std::to_string( shrink_to.y ) );
		SetHeight( shrink_to.y );
	}
}

} /* namespace object */
} /* namespace ui */
