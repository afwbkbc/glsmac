#include "Container.h"

#include "ui/geometry/Geometry.h"
#include "ui/UI.h"
#include "ui/Class.h"

#include "Surface.h"
#include "Panel.h"
#include "Text.h"
#include "Button.h"
#include "scene/actor/Cache.h"
#include "input/Event.h"

namespace ui {
namespace dom {

Container::Container( DOM_ARGS_T, const bool factories_allowed )
	: Area( DOM_ARGS_PASS_T )
	, m_cache( new scene::actor::Cache( "UI::Cache" ) ) {

	Actor( m_cache );

	m_geometry->m_on_effective_area_update = [ this ]( const geometry::Geometry::area_t& area ) {
		m_cache->SetEffectiveArea(
		{ (size_t)area.left, (size_t)area.top },
		{ (size_t)area.right, (size_t)area.bottom },
		area.zindex
		);
	};

	m_geometry->m_on_child_update = [ this ]() {
		m_cache->Update();
	};

	if ( factories_allowed ) {
		FACTORY( "surface", Surface );
		FACTORY( "panel", Panel );
		FACTORY( "text", Text );
		FACTORY( "button", Button );
	}
}

Container::~Container() {
	for ( const auto& it : m_children ) {
		delete it.second;
	}
}

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
		child->ProcessEvent( ctx, call_si, e );
	}
}

const bool Container::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
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
				object->ProcessEvent( ctx, call_si, e );
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
						object->ProcessEvent( ctx, call_si, e );
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
						object->ProcessEvent( ctx, call_si, e );
					}
				}
				m_processing_mouse_overs = false;
			}
			break;
		}
		default: {}
	}
	for ( const auto& child : m_children ) {
		if (
			child.second->ProcessEvent( ctx, call_si, event )
			) {
			return true;
		}
	}
	return Area::ProcessEventImpl( ctx, call_si, event );
}

#define FORWARD_CALL( _method, ... ) \
	auto forward_it = m_forwarded_properties.find( key ); \
	if ( forward_it != m_forwarded_properties.end() ) { \
		forward_it->second->_method( __VA_ARGS__ ); \
	} \
	else { \
		Object::_method( __VA_ARGS__ ); \
	}

void Container::WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) {
	auto forward_it = m_forwarded_properties.find( key );
	if ( forward_it != m_forwarded_properties.end() ) {
		if ( value.Get()->type == gse::type::Type::T_UNDEFINED && !m_classes.empty() ) {
			for ( const auto& c : m_classes ) {
				const auto it = c->GetProperties().find( key );
				if ( it != c->GetProperties().end() ) {
					forward_it->second->WrapSet( it->first, it->second, ctx, call_si );
					return;
				}
			}
		}
		forward_it->second->WrapSet( key, value, ctx, call_si );
	}
	else {
		Object::WrapSet( key, value, ctx, call_si );
	}
}

void Container::Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value, const property_flag_t flags, const f_on_set_t& f_on_set, const f_on_unset_t& f_on_unset ) {
	ASSERT_NOLOG( m_forwarded_properties.find( name ) == m_forwarded_properties.end(), "property '" + name + "' already exists (forwarded)" );
	Object::Property( ctx, call_si, name, type, default_value, flags, f_on_set, f_on_unset );
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
		target->WrapSet( it->first, it->second, ctx, call_si );
	}
}

void Container::Embed( Object* object ) {
	ASSERT_NOLOG( !m_is_initialized, "container already initialized" );
	m_embedded_objects.push_back( object );
}

void Container::Factory( GSE_CALLABLE, const std::string& name, const std::function< Object*( GSE_CALLABLE, const properties_t& ) >& f ) {
	Method( ctx, call_si, name, NATIVE_CALL( this, f ) {
		N_EXPECT_ARGS_MIN_MAX(0, 1);
		properties_t initial_properties = {};
		if ( arguments.size() == 1 ) {
			const auto& v = arguments.at(0).Get();
			if ( v->type != gse::type::Type::T_OBJECT ) {
				GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Expected properties object, got " + v->GetTypeString( v->type ) );
			}
			initial_properties = ((gse::type::Object*)v)->value;
		}
		auto* obj = f( ctx, call_si, initial_properties );
		ASSERT_NOLOG( obj, "object not created" );
		obj->InitAndValidate( ctx, call_si );
		m_children.insert({ obj->m_id, obj });
		return obj->Wrap( true );
	} ) );
}

void Container::OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const {
	FORWARD_CALL( OnPropertyChange, ctx, call_si, key, value )
}

void Container::OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const {
	FORWARD_CALL( OnPropertyRemove, ctx, call_si, key )
}

void Container::InitAndValidate( GSE_CALLABLE ) {
	for ( const auto& obj : m_embedded_objects ) {
		obj->InitAndValidate( ctx, call_si );
		m_children.insert({ obj->m_id, obj });
	}
	m_embedded_objects.clear();
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() && m_forwarded_properties.find( p.first ) == m_forwarded_properties.end() ) {
			GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Property '" + p.first + "' does not exist" );
		}
	}
	InitProperties( ctx, call_si );
}

void Container::SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value ) {
	FORWARD_CALL( SetPropertyFromClass, ctx, call_si, key, value );
}

void Container::UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) {
	// check in other classes
	for ( auto it = m_classes.rbegin() ; it != m_classes.rend() ; it++ ) {
		const auto value = (*it)->GetProperty( key, m_modifiers );
		if ( value.Get()->type != gse::type::Type::T_UNDEFINED ) {
			FORWARD_CALL( SetPropertyFromClass, ctx, call_si, key, value );
			return;
		}
	}
	// not found
	FORWARD_CALL( UnsetPropertyFromClass, ctx, call_si, key );
}

}
}
