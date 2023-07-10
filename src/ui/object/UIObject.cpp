#include <algorithm>

#include "UIObject.h"

#include "engine/Engine.h"
#include "../UI.h"

namespace ui {

using namespace event;

namespace object {

UIObject::UIObject( const std::string& class_name ) {
	m_position.left = 0;
	m_position.right = 0;
	m_position.top = 0;
	m_position.bottom = 0;
	m_size.aspect_ratio = 0;
	m_size.force_aspect_ratio = false;
	m_size.height = 0;
	m_size.width = 0;
	m_object_area.bottom = 0;
	m_object_area.height = 0;
	m_object_area.left = 0;
	m_object_area.right = 0;
	m_object_area.top = 0;
	m_object_area.width = 0;
	if ( !class_name.empty() ) {
		SetClass( class_name );
	}
}

UIObject::~UIObject() {
	if ( m_created ) {
		Destroy();
	}
	for ( auto& handlers : m_event_handlers ) {
		for ( auto& handler : handlers.second ) {
			if ( handler.second ) {
				DELETE( handler.first );
			}
		}
	}
	if ( !m_area_limits.source_objects.empty() ) {
		for ( auto& source_object : m_area_limits.source_objects ) {
			source_object->RemoveAreaLimitsChild( this );
		}
	}
}

void UIObject::Create() {
	
	DEBUG_STAT_INC( ui_elements_created );
	DEBUG_STAT_INC( ui_elements_active );
	
	m_style_modifiers = Style::M_NONE;
	
	m_created = true;
}

void UIObject::Destroy() {
	ASSERT( m_actors.empty(), "some actors still present on destruction" );
	
	HideDebugFrame();
	
	m_created = false;

	if ( m_is_focusable ) {
		g_engine->GetUI()->RemoveFromFocusableObjects( this );
	}
	
	DEBUG_STAT_INC( ui_elements_destroyed );
	DEBUG_STAT_DEC( ui_elements_active );
}

void UIObject::Iterate() {
	RealignMaybe();
}

void UIObject::Align() {
	UpdateAreaLimits();
}

void UIObject::Draw() {
	ApplyStyleIfNeeded();
	RealignMaybe();
	Refresh();
}

UIObject *UIObject::GetParentObject() const {
	return m_parent_object;
}

void UIObject::SetParentObject( UIContainer *parent_object ) {
	ASSERT( !m_parent_object, "parent object already set" );
	m_parent_object = parent_object;
	m_depth = m_parent_object->m_depth + 1;
	UpdateZIndex();
	UpdateObjectArea();
}

void UIObject::SetZIndex( float z_index ) {
	ASSERT( z_index >= 0.1 && z_index <= 0.9, "invalid z-index " + std::to_string( z_index ) );
	if ( m_z_index != z_index ) {
		m_z_index = z_index;
		UpdateZIndex();
	}
}

void UIObject::SetAbsoluteZIndex( float absolute_z_index ) {
	if ( absolute_z_index != m_absolute_z_index ) {
		m_absolute_z_index = absolute_z_index;
		for (auto& actor : m_actors) {
			actor->SetPositionZ( m_absolute_z_index );
		}
		Realign();
	}
}

void UIObject::UpdateZIndex() {
	float new_absolute_z_index = std::pow( 0.1, m_depth ) * m_z_index;
	if ( m_parent_object ) {
		new_absolute_z_index += m_parent_object->m_absolute_z_index;
	}
	SetAbsoluteZIndex( new_absolute_z_index );
}

scene::Scene *UIObject::GetSceneOfActor( const Actor *actor ) const {
	scene::Scene *scene;
	switch ( actor->GetType() ) {
		case ( Actor::TYPE_MESH ):
			scene = g_engine->GetUI()->GetShapeScene( ((actor::Mesh*)actor)->GetMesh() );
			break;
		case ( Actor::TYPE_TEXT ):
			scene = g_engine->GetUI()->GetTextScene();
			break;
		default:
			THROW( "unsupported actor type" );
	}
	return scene;
}

void UIObject::ApplyStyle() {

	// Spammy
	/*if ( m_style ) {
		Log( "Applying style class '" + m_style_class + "' (modifiers: " + to_string( m_style_modifiers ) + ")" );
	}
	if ( !m_parent_style_attributes.empty() ) {
		Log( "Using " + to_string( m_parent_style_attributes.size() ) + " forwarded style attributes from parent (modifiers: " + to_string( m_style_modifiers ) + ")" );
	}*/
	
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
	if ( Has( Style::A_Z_INDEX ) ) {
		SetZIndex( Get( Style::A_Z_INDEX ) );
	}

}

void UIObject::IgnoreStyleAttribute( const Style::attribute_type_t type ) {
	if ( !m_is_applying_style ) {
		m_ignore_style_attributes.insert( type );
	}
}

void UIObject::ReloadStyle() {
	if ( m_style_loaded ) {
		m_style_loaded = false;
	}
}

void UIObject::ForwardStyleAttribute( const Style::attribute_type_t src_type, const Style::attribute_type_t dst_type ) {
	m_parent_style_attributes[ dst_type ] = src_type;
	ReloadStyle();
}

void UIObject::ForwardStyleAttribute( const Style::attribute_type_t type ) {
	ForwardStyleAttribute( type, type );
}

void UIObject::ForwardStyleAttributesV( const std::vector< Style::attribute_type_t > types ) {
	for ( auto& type : types ) {
		m_parent_style_attributes[ type ] = type;
	}
	ReloadStyle();
}

void UIObject::ForwardStyleAttributesM( const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > types ) {
	for ( auto& type : types ) {
		m_parent_style_attributes[ type.second ] = type.first;
	}
	ReloadStyle();
}

void UIObject::ForwardAllStyleAttributes() {
	m_forward_all_style_attributes = true;
	ReloadStyle();
}

void UIObject::SetParentStyleObject( const UIContainer* object ) {
	ASSERT( !m_parent_style_object, "parent style object already set" );
	m_parent_style_object = object;
}

bool UIObject::Has( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const {
	if ( m_ignore_style_attributes.find( attribute_type ) != m_ignore_style_attributes.end() ) {
		return false;
	}
	if ( m_style && m_style->Has( attribute_type, style_modifiers ) ) {
		return true;
	}
	const UIContainer* style_object = m_parent_style_object ? m_parent_style_object : m_parent_object;
	if ( m_forward_all_style_attributes ) {
		ASSERT( style_object, "parent style attributes requested but parent is not set" );
		return style_object->Has( attribute_type, style_modifiers );
	}
	auto it = m_parent_style_attributes.find( attribute_type );
	if ( it != m_parent_style_attributes.end() ) {
		ASSERT( style_object, "parent style attributes requested but parent is not set" );
		return style_object->Has( it->second, style_modifiers );
	}
	return false;
}
bool UIObject::Has( const Style::attribute_type_t attribute_type ) const {
	return Has( attribute_type, m_style_modifiers );
}

const float UIObject::Get( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const {
	ASSERT( Has( attribute_type, style_modifiers ), "attribute get without has" );
	if ( m_style && m_style->Has( attribute_type, style_modifiers ) ) {
		return m_style->Get( attribute_type, style_modifiers );
	}
	const UIContainer* style_object = m_parent_style_object ? m_parent_style_object : m_parent_object;
	ASSERT( style_object, "parent style attributes requested but parent is not set" );
	return style_object->Get( GetParentAttribute( attribute_type ), style_modifiers );
}
const float UIObject::Get( const Style::attribute_type_t attribute_type ) const {
	return Get( attribute_type, m_style_modifiers );
}

const Color UIObject::GetColor( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const {
	ASSERT( Has( attribute_type, style_modifiers ), "attribute get without has" );
	if ( m_style && m_style->Has( attribute_type, style_modifiers ) ) {
		return m_style->GetColor( attribute_type, style_modifiers );
	}
	const UIContainer* style_object = m_parent_style_object ? m_parent_style_object : m_parent_object;
	ASSERT( style_object, "parent style attributes requested but parent is not set" );
	return style_object->GetColor( GetParentAttribute( attribute_type ), style_modifiers );
}
const Color UIObject::GetColor( const Style::attribute_type_t attribute_type ) const {
	return GetColor( attribute_type, m_style_modifiers );
}

const void* UIObject::GetObject( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const {
	ASSERT( Has( attribute_type, style_modifiers ), "attribute get without has" );
	if ( m_style && m_style->Has( attribute_type, style_modifiers ) ) {
		return m_style->GetObject( attribute_type, style_modifiers );
	}
	const UIContainer* style_object = m_parent_style_object ? m_parent_style_object : m_parent_object;
	ASSERT( style_object, "parent style attributes requested but parent is not set" );
	return style_object->GetObject( GetParentAttribute( attribute_type ), style_modifiers );
}
const void* UIObject::GetObject( const Style::attribute_type_t attribute_type ) const {
	return GetObject( attribute_type, m_style_modifiers );
}

void UIObject::AddActor( Actor* actor ) {
	ASSERT( std::find( m_actors.begin(), m_actors.end(), actor ) == m_actors.end(), "duplicate actor add" );
	actor->SetPositionZ( m_absolute_z_index );
	if ( m_is_actually_visible ) {
		actor->Show();
	}
	else {
		actor->Hide();
	}
	m_actors.push_back( actor );
}

void UIObject::RemoveActor( Actor* actor ) {
	auto it = std::find( m_actors.begin(), m_actors.end(), actor );
	ASSERT( it != m_actors.end(), "actor to be removed not found" );
	m_actors.erase( it );
}

void UIObject::EnableActors() {
	UpdateAreaLimits();
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

const UIObject::coord_t UIObject::UnclampX( const coord_t value ) {
	return g_engine->GetUI()->UnclampX( value );
}

const UIObject::coord_t UIObject::UnclampY( const coord_t value ) {
	return g_engine->GetUI()->UnclampY( value );
}
	
const UIObject::vertex_t UIObject::ClampXY( const vertex_t value ) {
	return vertex_t( ClampX( value.x ), ClampY( value.y ) );
}

void UIObject::SetEventContexts( event_context_t contexts ) {
	m_event_contexts = contexts;
}

void UIObject::AddEventContexts( event_context_t contexts ) {
	m_event_contexts |= contexts;
}

void UIObject::SetOverriddenEventContexts( event_context_t contexts ) {
	m_overridden_event_contexts = contexts;
}

void UIObject::AddOverriddenEventContexts( event_context_t contexts ) {
	m_overridden_event_contexts |= contexts;
}

bool UIObject::HasEventContext( event_context_t context ) const {
	return ( m_event_contexts & context );
}

bool UIObject::IsEventContextOverridden( event_context_t context ) const {
	return ( m_overridden_event_contexts & context );
}
	
void UIObject::RealignNow() {
	if ( !m_are_realigns_blocked ) {
		m_is_realign_needed = false;
		const auto object_area_old = GetObjectArea();
		UpdateObjectArea();
		if ( m_created ) {
			Align();
			if ( m_parent_object ) {
				m_parent_object->GrowFromObjectMaybe( this );
				//m_parent_object->ShrinkToFitMaybe(); // TODO: make it work
			}
		}
		for ( auto& child : m_area_limits.child_objects ) {
			child->UpdateAreaLimits();
		}
		Refresh();
		const auto& object_area_new = GetObjectArea();
		if (
			round( object_area_old.width ) != round( object_area_new.width ) ||
			round( object_area_old.height ) != round( object_area_new.height )
		) {
			UIEvent::event_data_t d = {};
			Trigger( UIEvent::EV_RESIZE, &d );
		}
	}
}

void UIObject::Realign() {
	if ( !m_are_realigns_blocked ) {
		//Log( "Realigning" );
		m_is_realign_needed = true;
	}
}

void UIObject::RealignMaybe() {
	if ( m_is_realign_needed ) {
		RealignNow();
	}
}

void UIObject::Redraw() {
	if ( m_created ) {
		RealignMaybe();
		Draw();
	}
}

const UIObject::coord_t UIObject::GetLeft() const {
	return m_position.left;
}

const UIObject::coord_t UIObject::GetRight() const {
	return m_position.right;
}

const UIObject::coord_t UIObject::GetTop() const {
	return m_position.top;
}

const UIObject::coord_t UIObject::GetBottom() const {
	return m_position.bottom;
}
	
const UIObject::coord_t UIObject::GetWidth() const {
	return m_size.width;
}

const UIObject::coord_t UIObject::GetHeight() const {
	return m_size.height;
}

void UIObject::SetLeft( const coord_t px ) {
	m_stick_bits |= STICK_LEFT;
	if ( ! ( m_align & ALIGN_HCENTER ) ) {
		m_align = ( m_align & (~ALIGN_RIGHT) ) | ALIGN_LEFT;
	}
	if ( m_stick_bits & STICK_RIGHT ) {
		m_stick_bits &= ~( STICK_RIGHT | STICK_WIDTH );
	}
	if ( m_position.left != px ) {
		m_position.left = px;
		Realign();
	}
}

void UIObject::SetRight( const coord_t px ) {
	m_stick_bits |= STICK_RIGHT;
	if ( ! ( m_align & ALIGN_HCENTER ) ) {
		m_align = ( m_align & (~ALIGN_LEFT) ) | ALIGN_RIGHT;
	}
	if ( m_stick_bits & STICK_LEFT ) {
		m_stick_bits &= ~( STICK_LEFT | STICK_WIDTH );
	}
	if ( m_position.right != px ) {
		m_position.right = px;
		Realign();
	}
}

void UIObject::SetTop( const coord_t px ) {
	m_stick_bits |= STICK_TOP;
	if ( ! ( m_align & ALIGN_VCENTER ) ) {
		m_align = ( m_align & (~ALIGN_BOTTOM) ) | ALIGN_TOP;
	}
	if ( m_stick_bits & STICK_BOTTOM ) {
		m_stick_bits &= ~( STICK_BOTTOM | STICK_HEIGHT );
	}
	if ( m_position.top != px ) {
		m_position.top = px;
		Realign();
	}
}

void UIObject::SetBottom( const coord_t px ) {
	m_stick_bits |= STICK_BOTTOM;
	if ( ! ( m_align & ALIGN_VCENTER ) ) {
		m_align = ( m_align & (~ALIGN_TOP) ) | ALIGN_BOTTOM;
	}
	if ( m_stick_bits & STICK_TOP ) {
		m_stick_bits &= ~( STICK_TOP | STICK_HEIGHT );
	}
	if ( m_position.bottom != px ) {
		m_position.bottom = px;
		Realign();
	}
}

void UIObject::SetMargin( const coord_box_t px ) {
	if ( m_margin != px ) {
		m_margin = px;
		Realign();
	}
}

void UIObject::SetMargin( const coord_t px ) {
	SetMargin({ px, px, px, px });
}

const UIObject::coord_box_t& UIObject::GetMargin() const {
	return m_margin;
}

void UIObject::SetOverflowMargin( const coord_t px ) {
	if ( m_overflow_margin != px ) {
		m_overflow_margin = px;
		Realign();
	}
}

void UIObject::Maximize() {
	SetLeft( 0 );
	SetTop( 0 );
	SetRight( 0 );
	SetBottom( 0 );
}

void UIObject::SetWidth( const coord_t px ) {
	m_stick_bits |= STICK_WIDTH;
	if ( m_size.width != px ) {
		m_size.width = px;
		Realign();
	}
	IgnoreStyleAttribute( Style::A_WIDTH );
}

void UIObject::SetHeight( const coord_t px ) {
	m_stick_bits |= STICK_HEIGHT;
	if ( m_size.height != px ) {
		m_size.height = px;
		Realign();
	}
	IgnoreStyleAttribute( Style::A_HEIGHT );
}

void UIObject::ForceAspectRatio( const float aspect_ratio ) {
	if ( m_size.aspect_ratio != aspect_ratio ) {
		m_size.aspect_ratio = aspect_ratio;
		m_size.force_aspect_ratio = true;
		Realign();
	}
}

void UIObject::UpdateObjectArea() {
	//Log( "Stick bits = " + std::to_string( m_stick_bits ) );
	const auto* g = g_engine->GetGraphics();
	const auto gw = g->GetViewportWidth();
	const auto gh = g->GetViewportHeight();
	object_area_t object_area;
	if ( m_parent_object != NULL ) {
		const auto area = m_parent_object->GetInternalObjectArea();
		object_area.left = area.left + m_position.left;
		object_area.right = area.right - m_position.right;
		object_area.top = area.top + m_position.top;
		object_area.bottom = area.bottom - m_position.bottom;
		if ( m_stick_bits & STICK_WIDTH ) {
			if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
				coord_t parent_center = ( area.left + area.right ) / 2;
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
				coord_t parent_center = ( area.top + area.bottom ) / 2;
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
		
		object_area.width = object_area.right - object_area.left;
		object_area.height = object_area.bottom - object_area.top;
		
		if ( m_size.force_aspect_ratio ) {
			float current_aspect_ratio = (float) object_area.height / object_area.width;
			if ( current_aspect_ratio > m_size.aspect_ratio ) {
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
			else if ( current_aspect_ratio < m_size.aspect_ratio ) {
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
		object_area.right = gw;
		object_area.top = 0;
		object_area.bottom = gh;
		object_area.width = object_area.right;
		object_area.height = object_area.bottom;
	}
	
	if ( m_margin ) {
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
	}
	
	if ( object_area != m_object_area ) {
		
		m_object_area = object_area;
		
		if ( m_created ) {
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
#endif
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

void UIObject::SetOverflow( const overflow_t overflow ) {
	if ( m_overflow != overflow ) {
		m_overflow = overflow;
		Realign();
	}
}

const float UIObject::GetZIndex() const {
	return m_z_index;
}

const UIObject::overflow_t UIObject::GetOverflow() const {
	return m_overflow;
}

void UIObject::ProcessEvent( UIEvent* event ) {
	
	if ( m_are_events_blocked || !m_is_actually_visible ) {
		return;
	}
	
	if ( event->m_flags & UIEvent::EF_MOUSE ) {
		event->m_data.mouse.relative.x = event->m_data.mouse.absolute.x - m_object_area.left;
		event->m_data.mouse.relative.y = event->m_data.mouse.absolute.y - m_object_area.top;
	}

	bool is_processed = false;
	
	if (
		( event->m_type != UIEvent::EV_MOUSE_MOVE ) || // mouse move events are sent to all elements, but we need to process only those inside object area
		( !m_parent_object ) // root object should be able to send mousemove to global handlers
	) {
		is_processed = Trigger( event->m_type, &event->m_data );
	}
	
	if ( !is_processed ) {
		
		switch ( event->m_type ) {
			case UIEvent::EV_MOUSE_MOVE: {
				if ( HasEventContext( EC_MOUSEMOVE ) ) {
					if (
						!event->IsMouseOverHappened() &&
						IsPointInside( event->m_data.mouse.absolute.x, event->m_data.mouse.absolute.y )
					) {
						event->SetMouseOverHappened();
						is_processed = Trigger( event->m_type, &event->m_data );
						if ( !is_processed ) {
							if ( !( m_state & STATE_MOUSEOVER ) ) {
								m_state |= STATE_MOUSEOVER;
								AddStyleModifier( Style::M_HOVER );
								is_processed = Trigger( UIEvent::EV_MOUSE_OVER, &event->m_data );
								if ( !is_processed ) {
									is_processed = OnMouseOver( &event->m_data );
								}
							}
						}
						else {
							//is_processed = true; // ???
						}
					}
					else {
						if ( m_state & STATE_MOUSEOVER ) {
							m_state &= ~STATE_MOUSEOVER;
							RemoveStyleModifier( Style::M_HOVER );
							is_processed = Trigger( UIEvent::EV_MOUSE_OUT, &event->m_data );
							if ( !is_processed ) {
								is_processed = OnMouseOut( &event->m_data );
							}
						}
						else {
							//is_processed = true; // ???
						}
					}
				}
				break;
			}
			case UIEvent::EV_MOUSE_DOWN: {
				if ( m_is_focusable && !m_is_focused ) {
					g_engine->GetUI()->FocusObject( this );
				}
				if ( HasEventContext( EC_MOUSE ) ) {
					is_processed = OnMouseDown( &event->m_data );
				}
				break;
			}
			case UIEvent::EV_MOUSE_UP: {
				if ( HasEventContext( EC_MOUSE ) ) {
					is_processed = OnMouseUp( &event->m_data );
				}
				break;
			}
			/*case UIEvent::EV_MOUSECLICK: {
				if ( HasEventContext( EC_MOUSE ) ) {
					is_processed = OnMouseClick( &event->m_data );
				}
				break;
			}*/ // conflicts with button click, maybe not needed?
			case UIEvent::EV_KEY_DOWN: {
				if ( HasEventContext( EC_KEYBOARD ) || m_is_focused ) {
					is_processed = OnKeyDown( &event->m_data );
				}
				break;
			}
			case UIEvent::EV_KEY_UP: {
				if ( HasEventContext( EC_KEYBOARD ) ) {
					is_processed = OnKeyUp( &event->m_data );
				}
				break;
			}
			case UIEvent::EV_KEY_PRESS: {
				if ( HasEventContext( EC_KEYBOARD ) ) {
					is_processed = OnKeyPress( &event->m_data );
				}
				break;
			}
			default: {
				// nothing
			}
		}
	}
	
	if ( is_processed ) {
		event->SetProcessed();
	}
}

UIObject::vertex_t UIObject::GetAreaPosition() const {
	return { m_object_area.left, m_object_area.top };
}

const UIObject::object_area_t UIObject::GetObjectArea() {
	RealignMaybe();
	return m_object_area;
}

const UIObject::object_area_t UIObject::GetInternalObjectArea() {
	return GetObjectArea();
}

bool UIObject::IsPointInside( const ssize_t x, const ssize_t y ) const {
	const object_area_t object_area = HasEventContext( EC_PARENTAREA )
		? m_parent_object->GetInternalObjectArea()
		: m_object_area
	;
	return (
		x > object_area.left &&
		y > object_area.top &&
		x <= object_area.right &&
		y < object_area.bottom
	);
}

void UIObject::SetClass( const std::string& style_class ) {
	ASSERT( m_style_class.empty(), "style class already set to '" + m_style_class + "'" ); // TODO: make changeable?
	ASSERT( !m_style_loaded, "style '" + m_style_class + "' already loaded" );
	//Log("Setting style class '" + style_class + "'");
	m_style_class = style_class;
	ApplyStyleIfNeeded();
}

#ifdef DEBUG
void UIObject::ShowDebugFrame() {
	if ( !m_has_debug_frame ) {
		if ( m_is_actually_visible ) {
			g_engine->GetUI()->ShowDebugFrame( this );
		}
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
		if ( !m_style_class.empty() || !m_parent_style_attributes.empty() || m_forward_all_style_attributes ) {
			if ( !m_style && !m_style_class.empty() ) {
				m_style = g_engine->GetUI()->GetStyle( m_style_class );
			}
			BlockRealigns();
			m_is_applying_style = true;
			ApplyStyle();
			m_is_applying_style = false;
			UnblockRealigns();
			Realign();
		}
		m_style_loaded = true;
	}
}

void UIObject::AddStyleModifier( const Style::modifier_t modifier ) {
	ASSERT( !( m_style_modifiers & modifier ), "style modifier " + std::to_string( modifier ) + " already added" );
	m_style_modifiers |= modifier;
	m_is_applying_style = true;
	ApplyStyle();
	m_is_applying_style = false;
	Redraw();
}

void UIObject::RemoveStyleModifier( const Style::modifier_t modifier ) {
	ASSERT( (m_style_modifiers & modifier), "style modifier " + std::to_string( modifier ) + " already removed" );
	m_style_modifiers &= ~modifier;
	m_is_applying_style = true;
	ApplyStyle();
	m_is_applying_style = false;
	Redraw();
}

const bool UIObject::HasStyleModifier( const Style::modifier_t modifier ) const {
	return ( (m_style_modifiers & modifier ) == modifier );
}

const UIEventHandler* UIObject::On( const std::vector< UIEvent::event_type_t >& types, UIEventHandler::handler_function_t func ) {
	NEWV( handler, UIEventHandler, func );
	bool need_delete = true; // need to delete only once
	for ( auto& type : types ) {
		auto it = m_event_handlers.find( type );
		if ( it == m_event_handlers.end() ) {
			m_event_handlers[ type ] = {};
			it = m_event_handlers.find( type );
		}
		it->second.push_back({
			handler,
			need_delete
		});
		need_delete = false;
	}
	return handler;
}

const UIEventHandler* UIObject::On( const UIEvent::event_type_t type, UIEventHandler::handler_function_t func ) {
	return On( std::vector{ type }, func );
}

void UIObject::Off( const UIEventHandler* handler ) {
	for ( auto& handlers : m_event_handlers ) {
		auto it = handlers.second.begin();
		while ( it != handlers.second.end() ) {
			if ( it->first == handler ) {
				it = handlers.second.erase( it );
			}
			else {
				it++;
			}
		}
	}
	DELETE( handler );
}

bool UIObject::Trigger( const UIEvent::event_type_t type, const UIEvent::event_data_t* data ) {
	auto handlers = m_event_handlers.find( type );
	if ( handlers != m_event_handlers.end() ) {
		for ( auto& handler : handlers->second ) {
			if ( handler.first->Execute( type, data ) ) {
				return true; // processed
			}
		}
	}
	return false; // not processed
}

const Style::attribute_type_t UIObject::GetParentAttribute( const Style::attribute_type_t source_type ) const {
	if ( m_forward_all_style_attributes ) {
		return source_type;
	}
	auto it = m_parent_style_attributes.find( source_type );
	ASSERT( it != m_parent_style_attributes.end(), "could not get attribute neither from style nor from parent attributes" );
	ASSERT( m_parent_style_object ? m_parent_style_object : m_parent_object,  "parent is gone" );
	return it->second;
}

const std::string UIObject::GetStyleModifiersString() const {
	std::string str = " ";
#ifdef DEBUG
	if ( HasStyleModifier( Style::M_HOVER ) ) {
		str += "HOVER ";
	}
	if ( HasStyleModifier( Style::M_ACTIVE ) ) {
		str += "ACTIVE ";
	}
	if ( HasStyleModifier( Style::M_SELECTED ) ) {
		str += "SELECTED ";
	}
	str = "( " + str + " )";
#endif
	return str;
}

void UIObject::BlockRealigns() {
	ASSERT( !m_are_realigns_blocked, "realigns already blocked" );
	//Log( "blocking realigns" );
	m_are_realigns_blocked = true;
}

void UIObject::UnblockRealigns() {
	ASSERT( m_are_realigns_blocked, "realigns already unblocked" );
	//Log( "unblocking realigns" );
	m_are_realigns_blocked = false;
}

const std::string& UIObject::GetStyleClass() {
	return m_style_class;
}

const std::string UIObject::SubClass( const std::string& subclass ) {
	return !m_style_class.empty() ? m_style_class + subclass : "";
}

void UIObject::BlockEvents() {
	if ( !m_are_events_blocked ) {
		//Log( "blocking events" );
		m_are_events_blocked = true;
	}
}

void UIObject::UnblockEvents() {
	if ( m_are_events_blocked ) {
		//Log( "unblocking events" );
		m_are_events_blocked = false;
	}
}

void UIObject::Select() {
	AddStyleModifier( Style::M_SELECTED );
}

void UIObject::Deselect() {
	RemoveStyleModifier( Style::M_SELECTED );
}

void UIObject::Focus() {
	m_is_focused = true;
	AddStyleModifier( Style::M_SELECTED );
}

void UIObject::Defocus() {
	m_is_focused = false;
	RemoveStyleModifier( Style::M_SELECTED );
}

void UIObject::Show() {
	if ( !m_is_visible ) {
		m_is_visible = true;
		if ( !m_parent_object || m_parent_object->IsVisible() ) {
			ShowActors();
		}
	}
}

void UIObject::ShowActors() {
	if ( m_is_visible && !m_is_actually_visible ) {
		m_is_actually_visible = true;
		UpdateAreaLimits();
		for ( auto& actor : m_actors ) {
			actor->Show();
		}
#ifdef DEBUG
		if ( m_has_debug_frame ) {
			g_engine->GetUI()->ShowDebugFrame( this );
		}
#endif
		Refresh();
	}
}

void UIObject::Hide() {
	if ( m_is_visible ) {
		m_is_visible = false;
		HideActors();
	}
}

void UIObject::HideActors() {
	if ( m_is_actually_visible ) {
		m_is_actually_visible = false;
		if ( m_state & STATE_MOUSEOVER ) {
			m_state &= ~STATE_MOUSEOVER;
			RemoveStyleModifier( Style::M_HOVER );
			UIEvent::event_data_t data = {};
			Trigger( UIEvent::EV_MOUSE_OUT, &data );
		}
		for ( auto& actor : m_actors ) {
			actor->Hide();
		}
#ifdef DEBUG
		if ( m_has_debug_frame ) {
			g_engine->GetUI()->HideDebugFrame( this );
		}
#endif
		Refresh();
	}
}

const bool UIObject::IsVisible() const {
	return m_is_actually_visible;
}

void UIObject::SetAreaLimits( const coord_box_t limits ) {
	m_area_limits.enabled = true;
	for ( auto& source_object : m_area_limits.source_objects ) {
		source_object->RemoveAreaLimitsChild( this );
	}
	m_area_limits.source_objects.clear();
	m_area_limits.limits = limits;
	UpdateAreaLimits();
}

void UIObject::SetAreaLimitsMaybe( const coord_box_t limits ) {
	if ( !m_area_limits.enabled || m_area_limits.source_objects.empty() ) {
		UIObject::SetAreaLimits( limits );
	}
}

void UIObject::SetAreaLimitsByObject( UIObject* source_object ) {
	ASSERT( source_object, "source object is null" );
	ASSERT( m_area_limits.source_objects.find( source_object ) == m_area_limits.source_objects.end(), "area limits source object already set" );
	//Log( "Setting area limits by object " + source_object->GetName() );
	m_area_limits.enabled = true;
	m_area_limits.source_objects.insert( source_object );
	source_object->AddAreaLimitsChild( this );
	m_area_limits.limits = {};
	UpdateAreaLimits();
}

void UIObject::AddAreaLimitsChild( UIObject* child_object ) {
	ASSERT( m_area_limits.child_objects.find( child_object ) == m_area_limits.child_objects.end(), "area limits child already added" );
	m_area_limits.child_objects.insert( child_object );
}

void UIObject::RemoveAreaLimitsChild( UIObject* child_object ) {
	auto it = m_area_limits.child_objects.find( child_object );
	ASSERT( it != m_area_limits.child_objects.end(), "area limits child not found" );
	m_area_limits.child_objects.erase( it );
}

void UIObject::ClearAreaLimits() {
	if ( m_area_limits.enabled ) {
		m_area_limits.enabled = false;
		UpdateAreaLimits();
	}
}

void UIObject::SetFocusable( bool is_focusable ) {
	if ( is_focusable != m_is_focusable ) {
		m_is_focusable = is_focusable;
		if ( m_is_focusable ) {
			Log( "setting focusable" );
			g_engine->GetUI()->AddToFocusableObjects( this );
		}
		else {
			Log( "setting not focusable" );
			g_engine->GetUI()->RemoveFromFocusableObjects( this );
		}
	}
}

void UIObject::Refresh() {
	g_engine->GetUI()->Redraw(); // TODO: partial updates
}

void UIObject::UpdateAreaLimits() {
	if ( m_area_limits.enabled && GetOverflow() != OVERFLOW_VISIBLE_ALWAYS ) {
		if ( !m_area_limits.source_objects.empty() ) {
			const auto* g = g_engine->GetGraphics();
			m_area_limits.limits = { // TODO: better z?
				0.0f,
				0.0f,
				(coord_t) g->GetViewportWidth(),
				(coord_t) g->GetViewportHeight()
			};
			for ( auto& source_object : m_area_limits.source_objects ) { // find the smallest box from all source objects
				const auto area = source_object->GetInternalObjectArea();
				m_area_limits.limits = {
					std::max< coord_t >( m_area_limits.limits.left, area.left + m_overflow_margin ),
					std::max< coord_t >( m_area_limits.limits.top, area.top + m_overflow_margin ),
					std::min< coord_t >( m_area_limits.limits.right, area.right - m_overflow_margin ),
					std::min< coord_t >( m_area_limits.limits.bottom, area.bottom - m_overflow_margin )
				};
			}
		}
		scene::actor::Mesh::area_limits_t limits = { // TODO: better z?
			{ ClampX( m_area_limits.limits.left ), ClampY( m_area_limits.limits.top ), -1.0f },
			{ ClampX( m_area_limits.limits.right ), ClampY( m_area_limits.limits.bottom ), 1.0f }
		};
		//Log( "Setting area limits to " + limits.first.ToString() + " -> " + limits.second.ToString() );
		for ( auto& actor : m_actors ) {
			actor->SetAreaLimits( limits );
		}
	}
	else {
		for ( auto& actor : m_actors ) {
			actor->RemoveAreaLimits();
		}
	}
}

} /* namespace object */
} /* namespace ui */
