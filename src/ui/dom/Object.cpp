#include "Object.h"

#include <atomic>

#include "Container.h"
#include "util/String.h"
#include "scene/actor/Actor.h"
#include "ui/UI.h"
#include "ui/Class.h"
#include "scene/Scene.h"
#include "input/Event.h"
#include "gse/type/Bool.h"

namespace ui {
namespace dom {

static std::atomic< id_t > s_next_id = 0;

Object::Object( DOM_ARGS_T )
	: m_ui( ui )
	, m_tag( "$" + tag )
	, m_id( ++s_next_id )
	, m_parent( parent )
	, m_initial_properties( properties ) {
	Property( ctx, call_si, "id", gse::type::Type::T_STRING );
	Property(
		ctx, call_si, "class", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& value ) {
			SetClass( ctx, call_si, ( (gse::type::String*)value.Get() )->value );
		}
	);
}

Object::~Object() {
	for ( const auto& actor : m_actors ) {
		m_ui->m_scene->RemoveActor( actor );
		delete actor;
	}
};

const gse::Value Object::Wrap( const bool dynamic ) {
	WRAPIMPL_PROPS
	};
	for ( const auto& p : m_properties ) {
		properties.insert(
			{
				p.first,
				p.second
			}
		);
	}
	return gse::Value(
		std::make_shared< gse::type::Object >(
			nullptr, properties, m_tag, this, dynamic
				? &Object::WrapSetStatic
				: nullptr
		)
	);
}

void Object::WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) {
	auto def_it = m_property_defs.find( key );
	if ( def_it == m_property_defs.end() ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' does not exist" );
	}
	if ( def_it->second.flags & PF_READONLY ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' is read-only" );
	}
	if ( value.Get()->type != gse::type::Type::T_UNDEFINED ) {
		SetProperty( ctx, call_si, &m_manual_properties, key, value );
		SetProperty( ctx, call_si, &m_properties, key, value );
	}
	else {
		UnsetProperty( ctx, call_si, &m_manual_properties, key );
		UnsetProperty( ctx, call_si, &m_properties, key );
	}
}

void Object::WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si ) {
	ASSERT_NOLOG( wrapobj, "wrapobj not set" );
	( (Object*)wrapobj )->WrapSet( key, value, ctx, call_si );
}

geometry::Geometry* const Object::GetGeometry() const {
	return nullptr;
}

const bool Object::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	if ( IsEventRelevant( event ) ) {
		if ( event.type == input::EV_MOUSE_OUT ) {
			int a = 5;
			a++;
		}
		return ProcessEventImpl( ctx, call_si, event );
	}
	else {
		return false;
	}
}

const bool Object::IsEventRelevant( const input::Event& event ) const {
	return m_supported_events.find( event.type ) != m_supported_events.end();
}

const bool Object::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	ASSERT_NOLOG( IsEventRelevant( event ), "event irrelevant" );
	const auto& event_type = event.GetTypeStr();
	if ( HasHandlers( event_type ) ) {
		gse::type::object_properties_t event_data = {};
		SerializeEvent( event, event_data );
		const auto result = Trigger( ctx, call_si, event_type, event_data, gse::type::Type::T_BOOL );
		return ( (gse::type::Bool*)result.Get() )->value;
	}
	return false;
}

void Object::Actor( scene::actor::Actor* actor ) {
	actor->SetPositionZ( 0.5f ); // TODO: proper zindex logic
	if ( m_parent ) {
		actor->SetCacheParent( m_parent->m_cache );
	}
	m_ui->m_scene->AddActor( actor );
	m_actors.push_back( actor );
}

void Object::Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value, const property_flag_t flags, const f_on_set_t& f_on_set, const f_on_unset_t& f_on_unset ) {
	ASSERT_NOLOG( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already exists" );
	ASSERT_NOLOG( m_property_defs.find( name ) == m_property_defs.end(), "property def '" + name + "' already exists" );
	m_property_defs.insert(
		{
			name,
			{
				type,
				flags,
				f_on_set,
				f_on_unset,
			}
		}
	);
	const auto& it = m_initial_properties.find( name );
	const gse::Value& v = it == m_initial_properties.end()
		? default_value
		: it->second;
	if ( default_value.Get()->type != gse::type::Type::T_UNDEFINED ) {
		m_default_properties.insert(
			{
				name,
				default_value
			}
		);
	}
	if ( it != m_initial_properties.end() ) {
		SetProperty( ctx, call_si, &m_manual_properties, name, it->second );
	}
	if ( v.Get()->type != gse::type::Type::T_UNDEFINED ) {
		SetProperty( ctx, call_si, &m_properties, name, v );
	}
}

void Object::Method( GSE_CALLABLE, const std::string& name, const gse::Value& callable ) {
	ASSERT_NOLOG( callable.Get()->type == gse::type::Type::T_CALLABLE, "method is not callable: " + callable.ToString() );
	Property( ctx, call_si, name, gse::type::Type::T_CALLABLE, callable, PF_READONLY );
}

void Object::Events( const std::unordered_set< input::event_type_t >& events ) {
	for ( const auto& event : events ) {
		ASSERT_NOLOG( m_supported_events.find( event ) == m_supported_events.end(), "event already handled: " + std::to_string( event ) );
		m_supported_events.insert( event );
	}
}

void Object::ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const {
	if ( !util::String::ParseColor( str, color ) ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property 'color' has invalid color format. Expected hex values ( #RRGGBB or #RRGGBBAA ), got: " + str );
	}
}

void Object::OnPropertyChange( GSE_CALLABLE, const std::string& key, const gse::Value& value ) const {
	const auto& def = m_property_defs.find( key );
	ASSERT_NOLOG( def != m_property_defs.end(), "property def not found" );
	if ( value.Get()->type != def->second.type ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' is expected to be " + value.Get()->GetTypeString( def->second.type ) + ", got " + value.GetTypeString() + ": " + value.ToString() );
	}
	for ( const auto& obj : m_wrapobjs ) {
		obj->value.insert_or_assign( key, value );
	}
	if ( def->second.f_on_set ) {
		def->second.f_on_set( ctx, call_si, value );
	}
}

void Object::OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const {
	const auto& def = m_property_defs.find( key );
	ASSERT_NOLOG( def != m_property_defs.end(), "property def not found" );
	if ( def->second.f_on_unset ) {
		def->second.f_on_unset( ctx, call_si );
	}
}

void Object::SerializeEvent( const input::Event& e, gse::type::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_KEY_DOWN: {
			if ( e.data.key.is_printable ) {
				obj.insert(
					{
						"key",
						VALUE( gse::type::String, std::string( 1, e.data.key.key ) )
					}
				);
			}
			obj.insert(
				{
					"code",
					VALUE( gse::type::String, e.GetKeyCodeStr() )
				}
			);
			break;
		}
		default: {
			ASSERT_NOLOG( false, "unknown event type: " + e.GetTypeStr() );
		}
	}
}

void Object::InitAndValidate( GSE_CALLABLE ) {
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() ) {
			GSE_ERROR( gse::EC.UI_ERROR, "Property '" + p.first + "' does not exist" );
		}
	}
	InitProperties( ctx, call_si );
}

void Object::InitProperties( GSE_CALLABLE ) {
	m_is_initialized = true;
	for ( const auto& it : m_initial_properties ) {
		const auto& v = it.second;
		OnPropertyChange( ctx, call_si, it.first, v );
	}
}

void Object::SetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key, const gse::Value& value ) {
	const auto& it = properties->find( key );
	if ( it == properties->end() ) {
		properties->insert(
			{
				key,
				value
			}
		);
		if ( m_is_initialized && properties == &m_properties ) {
			OnPropertyChange( ctx, call_si, key, value );
		}
	}
	else {
		if ( it->second != value ) {
			it->second = value;
			if ( m_is_initialized && properties == &m_properties ) {
				OnPropertyChange( ctx, call_si, key, value );
			}
		}
	}
}

void Object::UnsetProperty( gse::context::Context* ctx, const gse::si_t& call_si, properties_t* const properties, const std::string& key ) {
	const auto& it = properties->find( key );
	if ( it != properties->end() && it->second.Get()->type != gse::type::Type::T_UNDEFINED ) {
		if ( properties == &m_properties && m_class ) {
			const auto& it2 = m_class->GetProperties().find( key );
			if ( it2 != m_class->GetProperties().end() ) {
				SetProperty( ctx, call_si, properties, it2->first, it2->second );
				return;
			}
		}
		properties->erase( it );
		if ( m_is_initialized && properties == &m_properties ) {
			OnPropertyRemove( ctx, call_si, key );
		}
	}
}

void Object::UnsetClass( GSE_CALLABLE ) {
	ASSERT_NOLOG( m_class, "class not set" );
	if ( m_is_initialized ) {
		m_class->RemoveObject( ctx, call_si, this );
	}
	m_class = nullptr;
}

void Object::SetClass( GSE_CALLABLE, const std::string& name ) {
	if ( m_class ) {
		UnsetClass( ctx, call_si );
	}
	if ( name == "ball1" ) {
		int a = 5;
		a++;
	}
	m_class = m_ui->GetClass( name );
	if ( !m_class ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Class '" + name + "' does not exist" );
	}
	if ( m_is_initialized ) {
		m_class->AddObject( ctx, call_si, this );
	}
}

void Object::SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value ) {
	const auto& def_it = m_property_defs.find( key );
	if ( def_it == m_property_defs.end() ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' does not exist" );
	}
	if ( m_manual_properties.find( key ) == m_manual_properties.end() ) {
		SetProperty( ctx, call_si, &m_properties, key, value );
	}
}

void Object::UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) {
	const auto& def_it = m_property_defs.find( key );
	ASSERT_NOLOG( def_it != m_property_defs.end(), "property def not found: " + key );
	if ( m_manual_properties.find( key ) == m_manual_properties.end() ) {
		const auto& it = m_default_properties.find( key );
		if ( it != m_default_properties.end() ) {
			SetProperty( ctx, call_si, &m_properties, key, it->second );
		}
		else {
			UnsetProperty( ctx, call_si, &m_properties, key );
		}
	}
}

}
}
