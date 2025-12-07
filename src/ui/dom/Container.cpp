#include "Container.h"

#include "ui/geometry/Geometry.h"
#include "ui/UI.h"
#include "ui/Class.h"

#include "scene/actor/Cache.h"
#include "input/Event.h"

#include "Surface.h"
#include "Group.h"
#include "Panel.h"
#include "Text.h"
#include "Button.h"
#include "Input.h"
#include "Sound.h"
#include "ChoiceList.h"
#include "Select.h"
#include "Scrollbar.h"
#include "Scrollview.h"
#include "Listview.h"

namespace ui {
namespace dom {

Container::Container( DOM_ARGS_T, const bool factories_allowed, const bool has_body )
	: Area( DOM_ARGS_PASS_T )
	, m_cache( new scene::actor::Cache( "UI::Cache" ) )
	, m_has_body( has_body ) {

	Events( // TODO: refactor this
		{
			input::EV_KEY_DOWN,
			input::EV_MOUSE_SCROLL,
		}
	);

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
		GSE_CALL, "overflow", gse::Value::T_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto& value = ((gse::value::String*)v)->value;
			if ( value == "visible" ) {
				m_geometry->SetOverflowMode( geometry::Geometry::OM_VISIBLE );
			}
			else if ( value == "hidden" ) {
				m_geometry->SetOverflowMode( geometry::Geometry::OM_HIDDEN );
			}
			else {
				GSE_ERROR( gse::EC.UI_ERROR, "Invalid overflow value. Expected: visible or hidden, got: " + value );
			};
			return nullptr;
		},
		[ this ]( GSE_CALLABLE ) {
			m_geometry->SetOverflowMode( geometry::Geometry::OM_VISIBLE );
		}
	);

	if ( factories_allowed ) {
		FACTORY( "surface", Surface );
		FACTORY( "group", Group );
		FACTORY( "panel", Panel );
		FACTORY( "text", Text );
		FACTORY( "button", Button );
		FACTORY( "input", Input );
		FACTORY( "sound", Sound );
		FACTORY( "choicelist", ChoiceList );
		FACTORY( "select", Select );
		FACTORY( "scrollbar", Scrollbar );
		FACTORY( "scrollview", Scrollview );
		FACTORY( "listview", Listview );
	}

	Method( GSE_CALL, "clear", NATIVE_CALL( this ) {
		const auto children = m_children;
		for ( const auto& it : children ) {
			RemoveChild( GSE_CALL,it.second );
		}
		return nullptr;
	} ) );

}

void Container::ForwardFactories( GSE_CALLABLE, Object* child ) {
	for ( const auto& key : {
		"surface",
		"group",
		"panel",
		"window",
		"text",
		"button",
		"input",
		"sound",
	} ) {
		ForwardProperty( GSE_CALL, key, child );
	}
}

void Container::Embed( Object* object, const bool is_visible ) {
	ASSERT( !m_is_initialized, "container already initialized" );
	if ( object->m_parent != this ) {
		ASSERT( m_geometry, "object with custom parent but we have no geometry" );
		object->GetGeometry()->SetParent( m_geometry );
	}
	m_embedded_objects.push_back( { object, is_visible } );
}

Container::~Container() {
	// detach from children in case they live longer
	for ( const auto& object : m_children ) {
		object.second->Detach();
	}
	for ( const auto& object : m_embedded_objects ) {
		object.first->Detach();
	}
}

void Container::UpdateMouseOver( GSE_CALLABLE ) {
	const auto& mouse_coords = m_ui->GetLastMousePosition();
	m_processing_mouse_overs = true;
	if ( m_is_mouse_over ) {
		for ( auto it = m_children.rbegin() ; it != m_children.rend() ; it++ ) { // later children have priority
			auto* object = it->second;
			const auto* geometry = object->GetGeometry();
			if ( object->m_is_visible && geometry && geometry->Contains( mouse_coords ) ) {
				SetMouseOverChild( GSE_CALL, object, mouse_coords );
				m_processing_mouse_overs = false;
				return;
			}
		}
		if (
			m_mouse_over_object &&
			(
				!m_is_mouse_over ||
				!m_mouse_over_object->m_is_visible ||
				!m_mouse_over_object->GetGeometry()->Contains( mouse_coords )
			)
		) {
			SetMouseOverChild( GSE_CALL, nullptr, mouse_coords );
		}
	}
	m_processing_mouse_overs = false;
}

const bool Container::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	switch ( event.type ) {
		case input::EV_MOUSE_OUT: {
			if ( m_mouse_over_object ) {
				SetMouseOverChild( GSE_CALL, nullptr, { event.data.mouse.x, event.data.mouse.y } );
			}
			m_is_mouse_over = false;
			break;
		}
		case input::EV_MOUSE_OVER: {
			m_is_mouse_over = true;
			break;
		}
		case input::EV_MOUSE_MOVE: {
			m_is_mouse_over = m_is_visible && m_geometry->Contains( { event.data.mouse.x, event.data.mouse.y } );
			UpdateMouseOver( GSE_CALL );
			break;
		}
		default: {}
	}
	for ( auto it = m_children.rbegin() ; it != m_children.rend() ; it++ ) { // newer have priority
		const auto& child = it->second;
		if ( child->IsEventRelevant( event ) && child->ProcessEvent( GSE_CALL, event ) ) {
			return true;
		}
	}
	auto result = Area::ProcessEvent( GSE_CALL, event );
	if (
		m_has_body && // bodyless containers do not capture mouse events if no child caught it
		( event.flags & input::EF_MOUSE ) &&
		m_is_visible &&
		m_geometry->Contains( { event.data.mouse.x, event.data.mouse.y } )
	) {
		result = true; // only one child can handle mouse event, regardless of processing result
	}
	return result;
}

void Container::Destroy( GSE_CALLABLE ) {
	ASSERT( !m_processing_mouse_overs, "destruction is requested but still processing mouseovers" );
	const auto children = m_children;
	for ( const auto& it : children ) {
		RemoveChild( GSE_CALL,it.second );
	}
	ASSERT( m_children.empty(), "destruction is requested but was unable to remove all children" );
	ASSERT( !m_mouse_over_object, "destruction is requested but mouse over not empty" );

	Area::Destroy( GSE_CALL );
}

void Container::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Container" );

	GC_DEBUG_BEGIN( "children" );
	for ( const auto& it : m_children ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

	if ( !m_embedded_objects.empty() ) {
		GC_DEBUG_BEGIN( "embedded_objects" );
		for ( const auto& it : m_embedded_objects ) {
			GC_REACHABLE( it.first );
		}
		GC_DEBUG_END();
	}

	GC_DEBUG_END();
}

#define FORWARD_CALL( _method, ... ) \
	auto forward_it = m_forwarded_properties.find( key ); \
	if ( forward_it != m_forwarded_properties.end() ) { \
        if ( !m_children.empty() /* otherwise it means container is in the process of removal */ ) { \
			for ( const auto& it : forward_it->second )\
            	it.first->_method( GSE_CALL, it.second __VA_ARGS__ ); \
			\
        } \
	} \
	else { \
		Object::_method( GSE_CALL, key __VA_ARGS__ ); \
	}

void Container::WrapSet( const std::string& key, gse::Value* const value, GSE_CALLABLE ) {
	auto forward_it = m_forwarded_properties.find( key );
	if ( forward_it != m_forwarded_properties.end() ) {
		if ( value->type == gse::Value::T_UNDEFINED && !m_classes.empty() ) {
			for ( const auto& c : m_classes ) {
				const auto& properties = c->GetProperties();
				const auto it = properties.find( key );
				if ( it != properties.end() ) {
					for ( const auto& it2 : forward_it->second ) {
						it2.first->WrapSet( it2.second, it->second, GSE_CALL );
					}
					return;
				}
			}
		}
		for ( const auto& it : forward_it->second ) {
			it.first->WrapSet( it.second, value, GSE_CALL );
		}
		m_manual_properties.insert_or_assign( key, value );
		m_properties.insert_or_assign( key, value );
	}
	else {
		Object::WrapSet( key, value, GSE_CALL );
	}
}

void Container::Property( GSE_CALLABLE, const std::string& name, const gse::Value::type_t& type, gse::Value* default_value, const property_flag_t flags, const f_on_set_t& f_on_set, const f_on_unset_t& f_on_unset ) {
	ASSERT( m_forwarded_properties.find( name ) == m_forwarded_properties.end(), "property '" + name + "' already exists (forwarded)" );
	Object::Property( GSE_CALL, name, type, default_value, flags, f_on_set, f_on_unset );
}

void Container::ForwardProperty( GSE_CALLABLE, const std::string& name, Object* const target ) {
	ForwardProperty( GSE_CALL, name, name, target );
}

void Container::ForwardProperty( GSE_CALLABLE, const std::string& srcname, const std::string& dstname, Object* const target ) {
	// ? ASSERT( m_properties.find( srcname ) == m_properties.end(), "property '" + srcname + "' already taken (exists)" );
	//ASSERT( target->m_parent == this, "can only forward properties to direct children" ); // ?
	auto fp = m_forwarded_properties.find( srcname );
	if ( fp == m_forwarded_properties.end() ) {
		fp = m_forwarded_properties.insert({ srcname, {} }).first;
	}
	fp->second.push_back({ target, dstname });
	const auto& targetprop = target->m_properties.find( dstname );
	if ( targetprop != target->m_properties.end() ) {
		UpdateProperty( srcname, targetprop->second );
	}
	const auto& it = m_initial_properties.find( srcname );
	if ( it != m_initial_properties.end() ) {
		target->WrapSet( dstname, it->second, GSE_CALL );
		m_manual_properties.insert_or_assign( srcname, it->second );
		m_properties.insert_or_assign( srcname, it->second );
	}
}

void Container::Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f ) {
	Method( GSE_CALL, name, NATIVE_CALL( this, f ) {
		N_EXPECT_ARGS_MIN_MAX(0, 1);
		properties_t initial_properties = {};
		if ( arguments.size() == 1 ) {
			const auto& v = arguments.at(0);
			if ( v->type != gse::Value::T_OBJECT ) {
				GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Expected properties object, got " + v->GetTypeString() );
			}
			initial_properties = ((gse::value::Object*)v)->value;
		}
		if ( m_factory_owner->m_on_before_add_child ) {
			m_factory_owner->m_on_before_add_child( true );
		}
		auto* obj = f( GSE_CALL, initial_properties );
		ASSERT( obj, "object not created" );
		if ( m_factory_owner->m_on_add_child ) {
			m_factory_owner->m_on_add_child( obj, true );
		}
		obj->Show();
		obj->InitAndValidate( GSE_CALL );
		m_factory_owner->m_children.insert({ obj->m_id, obj });
		return obj->Wrap( GSE_CALL, true );
	} ) );
}

void Container::OnPropertyChange( GSE_CALLABLE, const std::string& key, gse::Value* const value ) {
	FORWARD_CALL( OnPropertyChange,, value )
}

void Container::OnPropertyRemove( GSE_CALLABLE, const std::string& key ) {
	FORWARD_CALL( OnPropertyRemove )
}

void Container::InitAndValidate( GSE_CALLABLE ) {
	for ( const auto& it : m_embedded_objects ) {
		AddChild( GSE_CALL, it.first, it.second );
	}
	m_embedded_objects.clear();
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() && m_forwarded_properties.find( p.first ) == m_forwarded_properties.end() ) {
			GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Property '" + p.first + "' does not exist" );
		}
	}
	InitProperties( GSE_CALL );
}

void Container::SetPropertyFromClass( GSE_CALLABLE, const std::string& key, gse::Value* const value, const class_modifier_t modifier ) {
	// check if property was set by any of previous classes with higher modifier
	for ( const auto& c : m_classes ) {
		const auto kv = c->GetProperty( key, m_modifiers );
		if ( kv.first && kv.second > modifier ) {
			return;
		}
	}
	FORWARD_CALL( SetPropertyFromClass,, value, modifier );
}

void Container::UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) {
	// check in other classes
	for ( auto it = m_classes.rbegin() ; it != m_classes.rend() ; it++ ) {
		const auto kv = (*it)->GetProperty( key, m_modifiers );
		if ( kv.first ) {
			FORWARD_CALL( SetPropertyFromClass,, kv.first, kv.second );
			return;
		}
	}
	// not found
	FORWARD_CALL( UnsetPropertyFromClass );
}

void Container::SetMouseOverChild( GSE_CALLABLE, Object* obj, const types::Vec2< ssize_t >& mouse_coords ) {
	if ( m_mouse_over_object != obj ) {
		input::Event e;
		e.data.mouse = { mouse_coords.x, mouse_coords.y };
		if ( m_mouse_over_object ) {
			e.SetType( input::EV_MOUSE_OUT );
			m_mouse_over_object->ProcessEvent( GSE_CALL, e );
		}
		m_mouse_over_object = obj;
		if ( m_mouse_over_object ) {
			e.SetType( input::EV_MOUSE_OVER );
			m_mouse_over_object->ProcessEvent( GSE_CALL, e );
		}
	}
}

void Container::AddChild( GSE_CALLABLE, Object* obj, const bool is_visible ) {
	ASSERT( m_children.find( obj->m_id ) == m_children.end(), "child already exists" );
	if ( m_on_before_add_child ) {
		m_on_before_add_child( false );
	}
	if ( m_on_add_child ) {
		m_on_add_child( obj, false );
	}
	if ( m_is_visible ) {
		obj->Show();
	}
	obj->InitAndValidate( GSE_CALL );
	m_children.insert({ obj->m_id, obj } );
	ASSERT( m_mouse_over_object != obj, "unexpected child mouseover" );
	const auto* geometry = obj->GetGeometry();
	if ( geometry ) {
		const auto& mousepos = m_ui->GetLastMousePosition();
		if ( geometry->Contains( mousepos ) ) {
			SetMouseOverChild( GSE_CALL, obj, mousepos );
		}
	}
	if ( m_is_visible && !is_visible ) {
		obj->Hide();
	}
}

void Container::RemoveChild( GSE_CALLABLE, Object* obj, const bool nodestroy ) {
	if ( m_children.find( obj->m_id ) == m_children.end() ) {
		// this can happen if globalized child deglobalizes during root object destruction
		return;
	}
	if ( m_on_remove_child ) {
		m_on_remove_child( obj );
	}
	m_children.erase( obj->m_id );
	if ( m_mouse_over_object == obj ) {
		SetMouseOverChild( GSE_CALL, nullptr, m_ui->GetLastMousePosition() );
	}
	if ( !nodestroy ) {
		obj->Destroy( GSE_CALL );
	}
}

}
}
