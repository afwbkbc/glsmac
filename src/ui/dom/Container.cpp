#include "Container.h"

#include "ui/geometry/Geometry.h"
#include "ui/UI.h"
#include "ui/Class.h"

#include "Surface.h"
#include "Group.h"
#include "Panel.h"
#include "Text.h"
#include "Button.h"
#include "Input.h"
#include "scene/actor/Cache.h"
#include "input/Event.h"

namespace ui {
namespace dom {

Container::Container( DOM_ARGS_T, const bool factories_allowed )
	: Area( DOM_ARGS_PASS_T )
	, m_cache( new scene::actor::Cache( "UI::Cache" ) ) {

	Actor( m_cache );

	GeometryHandler( GH_ON_EFFECTIVE_AREA_UPDATE, [ this ]() {
		const auto& area = m_geometry->GetEffectiveArea();
		m_cache->SetEffectiveArea(
		{ (size_t)area.left, (size_t)area.top },
		{ (size_t)area.right, (size_t)area.bottom },
		area.zindex
		);
	});

	GeometryHandler( GH_ON_CHILD_UPDATE, [ this ]() {
		m_cache->Update();
	});

	Property(
		GSE_CALL, "overflow", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			const auto& value = ((gse::type::String*)v.Get())->value;
			if ( value == "visible" ) {
				m_geometry->SetOverflowAllowed( true );
			}
			else if ( value == "hidden" ) {
				m_geometry->SetOverflowAllowed( false );
			}
			else {
				GSE_ERROR( gse::EC.UI_ERROR, "Invalid overflow value. Expected: visible or hidden, got: " + value );
			};
			return VALUE( gse::type::Undefined );
		},
		[ this ]( GSE_CALLABLE ) {
			m_geometry->SetOverflowAllowed( true );
		}
	);

	if ( factories_allowed ) {
		FACTORY( "surface", Surface );
		FACTORY( "group", Group );
		FACTORY( "panel", Panel );
		FACTORY( "text", Text );
		FACTORY( "button", Button );
		FACTORY( "input", Input );
	}
}

Container::~Container() {}

void Container::UpdateMouseOver( GSE_CALLABLE, Object* child ) {
	if ( m_processing_mouse_overs || m_children.find( child->m_id ) == m_children.end() ) {
		// maybe during initialization
		return;
	}
	const auto& last_mouse_pos = m_ui->GetLastMousePosition();
	const auto is_mouseover = child->GetGeometry()->Contains( last_mouse_pos );
	const auto was_mouseover = m_mouse_over_objects.find( child ) != m_mouse_over_objects.end();
	if ( is_mouseover != was_mouseover ) {
		input::Event e;
		if ( is_mouseover ) {
			e.SetType( input::EV_MOUSE_OVER );
			m_mouse_over_objects.insert( child );
		}
		else {
			e.SetType( input::EV_MOUSE_OUT );
			m_mouse_over_objects.erase( child );
		}
		e.data.mouse = { last_mouse_pos.x, last_mouse_pos.y };
		child->ProcessEvent( GSE_CALL, e );
	}
}

const bool Container::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	switch ( event.type ) {
		case input::EV_MOUSE_OUT: {
			input::Event e = {};
			m_processing_mouse_overs = true;
			for ( const auto& object : m_mouse_over_objects ) {
				ASSERT_NOLOG( object->GetGeometry(), "object has no geometry on mouseout" );
				if ( e.type == input::EV_NONE ) {
					e.SetType( input::EV_MOUSE_OUT );
					e.data.mouse = {
						event.data.mouse.x,
						event.data.mouse.y
					};
				}
				object->ProcessEvent( GSE_CALL, e );
			}
			m_processing_mouse_overs = false;
			m_mouse_over_objects.clear();
			break;
		}
		case input::EV_MOUSE_MOVE: {
			const types::Vec2< ssize_t > mouse_coords = {
				event.data.mouse.x,
				event.data.mouse.y
			};
			std::unordered_set< Object* > mouse_out_objects = {};
			std::unordered_set< Object* > mouse_over_objects = {};
			{
				input::Event e = {};
				m_processing_mouse_overs = true;
				for ( const auto& object : m_mouse_over_objects ) {
					ASSERT_NOLOG( object->GetGeometry(), "object has no geometry on mouseout" );
					if ( !object->GetGeometry()->Contains( mouse_coords ) ) {
						if ( e.type == input::EV_NONE ) {
							e.SetType( input::EV_MOUSE_OUT );
							e.data.mouse = event.data.mouse;
						}
						mouse_out_objects.insert( object );
						object->ProcessEvent( GSE_CALL, e );
					}
					else {
						mouse_over_objects.insert( object );
					}
				}
				if ( mouse_over_objects.size() != m_mouse_over_objects.size() ) {
					m_mouse_over_objects = mouse_over_objects;
				}
				m_processing_mouse_overs = false;
			}
			{
				input::Event e = {};
				m_processing_mouse_overs = true;
				for ( const auto& it : m_children ) {
					auto* object = it.second;
					const auto* geometry = object->GetGeometry();
					if (
						geometry &&
							m_mouse_over_objects.find( object ) == m_mouse_over_objects.end() &&
							mouse_out_objects.find( object ) == mouse_out_objects.end() &&
							geometry->Contains( mouse_coords )
						) {
						if ( e.type == input::EV_NONE ) {
							e.SetType( input::EV_MOUSE_OVER );
							e.data.mouse = event.data.mouse;
						}
						m_mouse_over_objects.insert( object );
						object->ProcessEvent( GSE_CALL, e );
					}
				}
				m_processing_mouse_overs = false;
			}
			break;
		}
		default: {}
	}
	ASSERT_NOLOG( !m_is_processing_children_events, "already processing children events" );
	m_is_processing_children_events = true;
	for ( auto it = m_children.rbegin() ; it != m_children.rend() ; it++ ) { // newer have priority
		const auto& child = it->second;
		if ( child->ProcessEvent( GSE_CALL, event ) ) {
			m_is_processing_children_events = false;
			ProcessPendingDeletes( GSE_CALL );
			return true;
		}
	}
	ASSERT_NOLOG( m_is_processing_children_events, "not processing children events" );
	m_is_processing_children_events = false;
	ProcessPendingDeletes( GSE_CALL );
	return Object::ProcessEvent( GSE_CALL, event );
}

void Container::Destroy( GSE_CALLABLE ) {
	ASSERT_NOLOG( !m_is_processing_children_events, "destruction is requested but still processing events" );
	ASSERT_NOLOG( !m_processing_mouse_overs, "destruction is requested but still processing mouseovers" );
	ASSERT_NOLOG( m_children_to_remove.empty(), "destruction is requested but still has pending children removals" );
	const auto children = m_children;
	for ( const auto& it : children ) {
		RemoveChild( GSE_CALL,it.second );
	}
	ASSERT_NOLOG( m_children_to_remove.empty(), "destruction is requested but got pending children removals" );
	ASSERT_NOLOG( m_children.empty(), "destruction is requested but was unable to remove all children" );
	ASSERT_NOLOG( m_mouse_over_objects.empty(), "destruction is requested but mouse overs not empty" );

	Area::Destroy( GSE_CALL );
}

#define FORWARD_CALL( _method, ... ) \
	auto forward_it = m_forwarded_properties.find( key ); \
	if ( forward_it != m_forwarded_properties.end() ) { \
        if ( !m_children.empty() /* otherwise it means container is in the process of removal */ ) { \
            forward_it->second->_method( __VA_ARGS__ ); \
        } \
	} \
	else { \
		Object::_method( __VA_ARGS__ ); \
	}

void Container::WrapSet( const std::string& key, const gse::Value& value, GSE_CALLABLE ) {
	auto forward_it = m_forwarded_properties.find( key );
	if ( forward_it != m_forwarded_properties.end() ) {
		if ( value.Get()->type == gse::type::Type::T_UNDEFINED && !m_classes.empty() ) {
			for ( const auto& c : m_classes ) {
				const auto it = c->GetProperties().find( key );
				if ( it != c->GetProperties().end() ) {
					forward_it->second->WrapSet( it->first, it->second, GSE_CALL );
					return;
				}
			}
		}
		forward_it->second->WrapSet( key, value, GSE_CALL );
		m_manual_properties.insert_or_assign( key, value );
		m_properties.insert_or_assign( key, value );
	}
	else {
		Object::WrapSet( key, value, GSE_CALL );
	}
}

void Container::Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value, const property_flag_t flags, const f_on_set_t& f_on_set, const f_on_unset_t& f_on_unset ) {
	ASSERT_NOLOG( m_forwarded_properties.find( name ) == m_forwarded_properties.end(), "property '" + name + "' already exists (forwarded)" );
	Object::Property( GSE_CALL, name, type, default_value, flags, f_on_set, f_on_unset );
}

void Container::ForwardProperty( GSE_CALLABLE, const std::string& name, Object* const target ) {
	ASSERT_NOLOG( m_forwarded_properties.find( name ) == m_forwarded_properties.end(), "property '" + name + "' already forwarded" );
	ASSERT_NOLOG( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already taken (exists)" );
	ASSERT_NOLOG( target->m_parent == this, "can only forward properties to direct children" );
	m_forwarded_properties.insert(
		{
			name,
			target
		}
	);
	const auto& it = m_initial_properties.find( name );
	if ( it != m_initial_properties.end() ) {
		target->WrapSet( it->first, it->second, GSE_CALL );
		m_manual_properties.insert_or_assign( it->first, it->second );
		m_properties.insert_or_assign( it->first, it->second );
	}
}

void Container::Embed( Object* object ) {
	ASSERT_NOLOG( !m_is_initialized, "container already initialized" );
	m_embedded_objects.push_back( object );
}

void Container::Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f ) {
	Method( GSE_CALL, name, NATIVE_CALL( this, f ) {
		N_EXPECT_ARGS_MIN_MAX(0, 1);
		properties_t initial_properties = {};
		if ( arguments.size() == 1 ) {
			const auto& v = arguments.at(0).Get();
			if ( v->type != gse::type::Type::T_OBJECT ) {
				GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Expected properties object, got " + v->GetTypeString( v->type ) );
			}
			initial_properties = ((gse::type::Object*)v)->value;
		}
		auto* obj = f( GSE_CALL, initial_properties );
		ASSERT_NOLOG( obj, "object not created" );
		obj->InitAndValidate( GSE_CALL );
		m_children.insert({ obj->m_id, obj });
		return obj->Wrap( true );
	} ) );
}

void Container::OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const {
	FORWARD_CALL( OnPropertyChange, GSE_CALL, key, value )
}

void Container::OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const {
	FORWARD_CALL( OnPropertyRemove, GSE_CALL, key )
}

void Container::InitAndValidate( GSE_CALLABLE ) {
	for ( const auto& obj : m_embedded_objects ) {
		obj->InitAndValidate( GSE_CALL );
		m_children.insert({ obj->m_id, obj });
	}
	m_embedded_objects.clear();
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() && m_forwarded_properties.find( p.first ) == m_forwarded_properties.end() ) {
			GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Property '" + p.first + "' does not exist" );
		}
	}
	InitProperties( GSE_CALL );
}

void Container::ProcessPendingDeletes( GSE_CALLABLE ) {
	for ( const auto& obj : m_children_to_remove ) {
		ASSERT_NOLOG( !m_is_processing_children_events, "can't process pending deletes within event processing" );
		RemoveChild( GSE_CALL, obj );
	}
	m_children_to_remove.clear();
}

void Container::SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value, const class_modifier_t modifier ) {
	// check if property was set by any of previous classes with higher modifier
	for ( const auto& c : m_classes ) {
		const auto kv = c->GetProperty( key, m_modifiers );
		if ( kv.first.Get()->type != gse::type::Type::T_UNDEFINED && kv.second > modifier ) {
			return;
		}
	}
	FORWARD_CALL( SetPropertyFromClass, GSE_CALL, key, value, modifier );
}

void Container::UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) {
	// check in other classes
	for ( auto it = m_classes.rbegin() ; it != m_classes.rend() ; it++ ) {
		const auto kv = (*it)->GetProperty( key, m_modifiers );
		if ( kv.first.Get()->type != gse::type::Type::T_UNDEFINED ) {
			FORWARD_CALL( SetPropertyFromClass, GSE_CALL, key, kv.first, kv.second );
			return;
		}
	}
	// not found
	FORWARD_CALL( UnsetPropertyFromClass, GSE_CALL, key );
}

void Container::RemoveChild( GSE_CALLABLE, Object* obj ) {
	const bool can_delete_immediately = !m_is_processing_children_events;
	if ( can_delete_immediately ) {
		ASSERT_NOLOG( m_children.find( obj->m_id ) != m_children.end(), "child not found" );
		m_children.erase( obj->m_id );
		const auto& it = m_mouse_over_objects.find( obj );
		if ( it != m_mouse_over_objects.end() ) {
			const auto& last_mouse_pos = m_ui->GetLastMousePosition();
			input::Event e;
			e.SetType( input::EV_MOUSE_OUT );
			m_mouse_over_objects.erase( obj );
			e.data.mouse = { last_mouse_pos.x, last_mouse_pos.y };
			obj->ProcessEvent( GSE_CALL, e );
		}
		obj->Destroy( GSE_CALL );
	}
	else {
		ASSERT_NOLOG( m_children_to_remove.find( obj ) == m_children_to_remove.end(), "child already queued for deletion" );
		m_children_to_remove.insert( obj );
	}
}

}
}
