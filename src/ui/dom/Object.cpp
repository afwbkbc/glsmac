#include "Object.h"

#include <atomic>
#include <algorithm>

#include "Container.h"
#include "util/String.h"
#include "scene/actor/Actor.h"
#include "ui/UI.h"
#include "ui/Class.h"
#include "scene/Scene.h"
#include "input/Event.h"
#include "gse/value/Bool.h"
#include "util/LogHelper.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "gse/callable/Native.h"
#include "gse/value/Object.h"
#include "Root.h"

#include "Object.colormap.cpp"

namespace ui {
namespace dom {

static std::atomic< id_t > s_next_id = 0;

Object::Object( DOM_ARGS_T )
	: gse::GCWrappable( ui->GetGCSpace() )
	, m_ui( ui )
	, m_gc_space( ui->GetGCSpace() )
	, m_tag( "$" + tag )
	, m_id( ++s_next_id )
	, m_parent( parent )
	, m_initial_properties( properties ) {
	Property( GSE_CALL, "id", gse::Value::T_STRING );

	Property(
		GSE_CALL, "class", gse::Value::T_STRING, nullptr, PF_NONE, [ this ]( GSE_CALLABLE, gse::Value* const value ) {
			const auto names = util::String::Split( ( (gse::value::String*)value )->value, ' ' );
			// check for duplicates
			std::unordered_set< std::string > names_set = {};
			for ( const auto& n : names ) {
				if ( names_set.find( n ) != names_set.end() ) {
					GSE_ERROR( gse::EC.UI_ERROR, "Duplicate class: " + n );
				}
				names_set.insert( n );
			}
			SetClasses( GSE_CALL, names );
		}
	);
	Method( GSE_CALL, "addclass", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( name, 0, String );
		ASSERT( m_properties.at("class")->type == gse::Value::T_STRING, "class is not string" );
		auto names = util::String::Split( ((gse::value::String*)m_properties.at("class"))->value, ' ' );
		if ( std::find( names.begin(), names.end(), name ) == names.end() ) {
			names.push_back( name );
			UpdateProperty( "class", VALUE( gse::value::String,, util::String::Join( names, ' ' ) ) );
			SetClasses( GSE_CALL, names );
		}
		return nullptr;
	} ) );
	Method( GSE_CALL, "removeclass", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( name, 0, String );
		ASSERT( m_properties.at( "class" )->type == gse::Value::T_STRING, "class is not string" );
		const auto names = util::String::Split( ( (gse::value::String*)m_properties.at( "class" ) )->value, ' ' );
		if ( std::find( names.begin(), names.end(), name ) != names.end() ) {
			std::vector< std::string > names_new = {};
			names_new.reserve( names.size() - 1 );
			for ( const auto& n : names ) {
				if ( n != name ) {
					names_new.push_back( n );
				}
			}
			UpdateProperty( "class", VALUE( gse::value::String,, util::String::Join( names_new, ' ' ) ) );
			SetClasses( GSE_CALL, names_new );
		}
		return nullptr;
	} ) );
	Method( GSE_CALL, "remove", NATIVE_CALL( this ) {
		ASSERT( m_parent, "remove without parent" );
		m_parent->RemoveChild( GSE_CALL, this );
		return nullptr;
	} ) );
}

Object::~Object() {
	ASSERT( m_is_destroyed, "object was not destroyed properly!" );
	if ( !m_is_destroyed ) {
		util::LogHelper::Println( "WARNING: object was not destroyed properly!" );
	}
	if ( !m_actors.empty() ) {
		for ( const auto& actor : m_actors ) {
			delete actor;
		}
	}
};

gse::Value* const Object::Wrap( GSE_CALLABLE, const bool dynamic ) {
	if ( !m_wrapobj ) {
		WRAPIMPL_PROPS
		WRAPIMPL_TRIGGERS
		};
		for ( const auto& p : m_properties ) {
			properties.insert(
				{
					p.first,
					p.second
				}
			);
		}
		m_wrapobj = new gse::value::Object(
			GSE_CALL, properties, m_tag, this, dynamic
				? &Object::WrapSetStatic
				: nullptr
		);
	}
	return m_wrapobj;
}

void Object::WrapSet( const std::string& key, gse::Value* const value, GSE_CALLABLE ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	auto def_it = m_property_defs.find( key );
	if ( def_it == m_property_defs.end() ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' does not exist" );
	}
	if ( def_it->second.flags & PF_READONLY ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' is read-only" );
	}
	if ( value && value->type != gse::Value::T_UNDEFINED ) {
		SetProperty( GSE_CALL, &m_manual_properties, key, value );
		SetProperty( GSE_CALL, &m_properties, key, value );
	}
	else {
		UnsetProperty( GSE_CALL, &m_manual_properties, key );
		UnsetProperty( GSE_CALL, &m_properties, key );
	}
}

void Object::WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE ) {
	ASSERT( wrapobj, "wrapobj not set" );
	( (Object*)wrapobj )->WrapSet( key, value, GSE_CALL );
}

geometry::Geometry* const Object::GetGeometry() const {
	return nullptr;
}

const bool Object::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	if ( IsEventRelevant( event ) ) {
		switch ( event.type ) {
			case input::EV_MOUSE_OVER: {
				AddModifier( GSE_CALL, CM_HOVER );
				break;
			}
			case input::EV_MOUSE_OUT: {
				RemoveModifier( GSE_CALL, CM_HOVER );
				break;
			}
			default: {}
		}
		return ProcessEventImpl( GSE_CALL, event );
	}
	else {
		return false;
	}
}

void Object::Destroy( GSE_CALLABLE ) {
	ASSERT( !m_is_destroyed, "already destroyed" );
	if ( m_is_globalized ) {
		Deglobalize( GSE_CALL );
	}
	Hide();
	SetClasses( GSE_CALL, {} );
	if ( m_is_iterable_set ) {
		m_ui->RemoveIterable( this );
	}
	for ( const auto& actor : m_actors ) {
		m_ui->m_scene->RemoveActor( actor );
	}
	m_is_destroyed = true;
}

void Object::Show() {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	if ( !m_is_visible ) {
		for ( const auto& actor : m_actors ) {
			actor->Show();
		}
		m_is_visible = true;
	}
}

void Object::Hide() {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	if ( m_is_visible ) {
		for ( const auto& actor : m_actors ) {
			actor->Hide();
		}
		m_is_visible = false;
	}
}

void Object::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gse::GCWrappable::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Object" );

	if ( m_wrapobj ) {
		GC_DEBUG_BEGIN( "wrapobj" );
		GC_REACHABLE( m_wrapobj );
		GC_DEBUG_END();
	}

	GC_DEBUG_BEGIN( "properties");
	for ( const auto& p : m_properties ) {
		GC_REACHABLE( p.second );
	}
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "default_properties");
	for ( const auto& p : m_default_properties ) {
		GC_REACHABLE( p.second );
	}
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "manual_properties");
	for ( const auto& p : m_manual_properties ) {
		GC_REACHABLE( p.second );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

const bool Object::IsEventRelevant( const input::Event& event ) const {
	return
		!m_is_destroyed &&
		m_is_visible &&
		m_supported_events.find( event.type ) != m_supported_events.end()
	;
}

const bool Object::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	if ( IsEventRelevant( event ) ) {
		const auto& event_type = event.GetTypeStr();
		if ( HasHandlers( event_type ) ) {
			gse::value::object_properties_t event_data = {};
			WrapEvent( GSE_CALL, event, event_data );
			const auto result = Trigger( GSE_CALL, event_type, ARGS( event_data ), gse::Value::T_BOOL );
			return ( (gse::value::Bool*)result )->value;
		}
	}
	return false;
}

void Object::UpdateProperty( const std::string& k, gse::Value* const v ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	m_properties.insert_or_assign( k, v );
	if ( m_wrapobj ) {
		m_wrapobj->Assign( k, v );
	}
}

void Object::Actor( scene::actor::Actor* actor ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	actor->SetPositionZ( 0.5f );
	if ( m_parent ) {
		actor->SetCacheParent( m_parent->m_cache );
	}
	m_ui->m_scene->AddActor( actor );
	m_actors.push_back( actor );
}

void Object::Property( GSE_CALLABLE, const std::string& name, const gse::Value::type_t& type, gse::Value* const default_value, const property_flag_t flags, const f_on_set_t& f_on_set, const f_on_unset_t& f_on_unset ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	ASSERT( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already exists" );
	ASSERT( m_property_defs.find( name ) == m_property_defs.end(), "property def '" + name + "' already exists" );
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
	gse::Value* const v = it == m_initial_properties.end()
		? default_value
			? default_value
			: nullptr
		: it->second;
	if ( default_value ) {
		m_default_properties.insert(
			{
				name,
				default_value
			}
		);
	}
	if ( it != m_initial_properties.end() ) {
		SetProperty( GSE_CALL, &m_manual_properties, name, it->second );
	}
	if ( v ) {
		SetProperty( GSE_CALL, &m_properties, name, v );
	}
}

void Object::Method( GSE_CALLABLE, const std::string& name, gse::Value* const callable ) {
	ASSERT( callable->type == gse::Value::T_CALLABLE, "method is not callable: " + callable->ToString() );
	Property( GSE_CALL, name, gse::Value::T_CALLABLE, callable, PF_READONLY );
}

void Object::Events( const std::unordered_set< input::event_type_t >& events ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	for ( const auto& event : events ) {
		ASSERT( m_supported_events.find( event ) == m_supported_events.end(), "event already handled: " + std::to_string( event ) );
		m_supported_events.insert( event );
	}
}

void Object::Iterable( const std::function< void() >& f ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	ASSERT( !m_is_iterable_set, "iterable already set" );
	m_is_iterable_set = true;
	m_ui->AddIterable( this, f );
}

const bool Object::TryParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const {
	// color names
	const auto it = s_color_map.find( str );
	if ( it != s_color_map.end() ) {
		color = it->second;
		return true;
	}
	const auto is_rgb = str.substr( 0, 4 ) == "rgb(";
	const auto is_rgba = str.substr( 0, 5 ) == "rgba(";
	if ( is_rgb || is_rgba ) {
		const auto f_error = [ &ctx, &si, &ep, &is_rgb, &str ]() {
			GSE_ERROR( gse::EC.UI_ERROR, (std::string)"Property has invalid color format. Method " + ( is_rgb ? "rgb expects 3" : "rgba expects 4" ) + " values (0 to 255), got: " + str );
		};
		const auto begin = is_rgb ? 4 : 5;
		const auto end = str.rfind( ')' );
		if ( end != std::string::npos ) {
			const auto parts = util::String::Split( str.substr(begin, end - begin ), ',' );
			if ( parts.size() != ( is_rgb ? 3 : 4 ) ) {
				f_error();
			}
			long int r, g, b, a;
			if (
				!util::String::ParseInt( parts.at( 0 ), r ) || r < 0 || r > 255 ||
				!util::String::ParseInt( parts.at( 1 ), g ) || g < 0 || g > 255 ||
				!util::String::ParseInt( parts.at( 2 ), b ) || b < 0 || b > 255 ||
				( is_rgba && ( !util::String::ParseInt( parts.at( 3 ), a ) || a < 0 || a > 255 ) )
			) {
				f_error();
			}
			color = is_rgb
				? types::Color::FromRGB( r, g, b )
				: types::Color::FromRGBA( r, g, b, a )
			;
			return true;
		}
	}
	// hex values
	return util::String::ParseColorHex( str, color );
}

void Object::ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const {
	if ( !TryParseColor( GSE_CALL, str, color ) ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property has invalid color format. Expected color name ('green', 'red', ...), rgb values ('rgb(0,255,0)', 'rgb(100,100,20)', ...) or hex value ('#ff0000', '#227748', ...), got: " + str );
	}
}

void Object::OnPropertyChange( GSE_CALLABLE, const std::string& key, gse::Value* const value ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	const auto& def = m_property_defs.find( key );
	ASSERT( def != m_property_defs.end(), "property def not found" );
	const auto t1 = def->second.type;
	if ( t1 != gse::Value::T_NOTHING ) {
		const auto t2 = value->type;
		if ( t1 != t2 ) {
			GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' is expected to be " + gse::Value::GetTypeStringStatic( def->second.type ) + ", got " + value->GetTypeString() + ": " + value->ToString() );
		}
	}
	for ( const auto& obj : m_wrapobjs ) {
		obj->Assign( key, value );
	}
	if ( def->second.f_on_set ) {
		def->second.f_on_set( GSE_CALL, value );
	}
}

void Object::OnPropertyRemove( GSE_CALLABLE, const std::string& key ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	const auto& def = m_property_defs.find( key );
	ASSERT( def != m_property_defs.end(), "property def not found" );
	{
		for ( const auto& obj : m_wrapobjs ) {
			obj->Assign( key, nullptr );
		}
	}
	if ( def->second.f_on_unset ) {
		def->second.f_on_unset( GSE_CALL );
	}
}

void Object::WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_KEY_DOWN: {
			if ( e.data.key.is_printable ) {
				obj.insert(
					{
						"key",
						VALUE( gse::value::String,, std::string( 1, e.data.key.key ) )
					}
				);
			}
			obj.insert(
				{
					"code",
					VALUE( gse::value::String,, e.GetKeyCodeStr() )
				}
			);
			gse::value::object_properties_t modifier_props = {};
			if ( e.data.key.modifiers & input::KM_SHIFT ) {
				modifier_props.insert({"shift", VALUE( gse::value::Bool,, true )});
			}
			if ( e.data.key.modifiers & input::KM_CTRL ) {
				modifier_props.insert({"ctrl", VALUE( gse::value::Bool,, true )});
			}
			if ( e.data.key.modifiers & input::KM_ALT ) {
				modifier_props.insert({"alt", VALUE( gse::value::Bool,, true )});
			}
			obj.insert({"modifiers", VALUE( gse::value::Object,, ctx, {}, ep, modifier_props ) } );
			break;
		}
		default: {
			ASSERT( false, "unknown event type: " + e.GetTypeStr() );
		}
	}
}

void Object::AddModifier( GSE_CALLABLE, const class_modifier_t modifier ) {
	if ( !m_is_destroyed && m_modifiers.find( modifier ) == m_modifiers.end() ) {
		m_modifiers.insert( modifier );
		for ( const auto& c : m_classes ) {
			c->AddObjectModifier( GSE_CALL, this, modifier );
		}
	}
}

void Object::RemoveModifier( GSE_CALLABLE , const class_modifier_t modifier ) {
	if ( !m_is_destroyed && m_modifiers.find( modifier ) != m_modifiers.end() ) {
		m_modifiers.erase( modifier );
		for ( const auto& c : m_classes ) {
			c->RemoveObjectModifier( GSE_CALL, this, modifier );
		}
	}
}

void Object::Globalize( GSE_CALLABLE, const std::function< void() >& f_on_deglobalize ) {
	if ( m_is_globalized ) {
		Deglobalize( GSE_CALL );
	}
	ASSERT( m_parent == m_ui->GetRoot(), "can only globalize children of root object" );
	m_ui->SetGlobalSingleton( GSE_CALL, this, f_on_deglobalize );
	m_is_globalized = true;
}

void Object::Deglobalize( GSE_CALLABLE ) {
	if ( m_is_globalized ) {
		ASSERT( m_parent == m_ui->GetRoot(), "can only deglobalize children of root object" );
		m_ui->RemoveGlobalSingleton( GSE_CALL, this );
		m_is_globalized = false;
	}
}

void Object::InitAndValidate( GSE_CALLABLE ) {
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() ) {
			GSE_ERROR( gse::EC.UI_ERROR, "Property '" + p.first + "' does not exist" );
		}
	}
	InitProperties( GSE_CALL );
}

void Object::InitProperties( GSE_CALLABLE ) {
	m_is_initialized = true;
	for ( const auto& it : m_initial_properties ) {
		const auto& v = it.second;
		OnPropertyChange( GSE_CALL, it.first, v );
	}
}

void Object::Detach() {
	ASSERT( m_parent, "parent not set" );
	m_parent = nullptr;
}

void Object::SetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key, gse::Value* const value ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	const auto& it = properties->find( key );
	if ( it == properties->end() ) {
		if ( properties == &m_properties ) {
			UpdateProperty( key, value );
		}
		else {
			properties->insert(
				{
					key,
					value
				}
			);
		}
		if ( m_is_initialized && properties == &m_properties ) {
			OnPropertyChange( GSE_CALL, key, value );
		}
	}
	else {
		if ( ( ( it->second == nullptr ) != ( value == nullptr ) ) || ( value && ( *it->second != *value ) ) ) {
			it->second = value;
			if ( m_is_initialized && properties == &m_properties ) {
				OnPropertyChange( GSE_CALL, key, value );
			}
		}
	}
}

void Object::UnsetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	const auto& it = properties->find( key );
	if ( it != properties->end() && it->second->type != gse::Value::T_UNDEFINED ) {
		if ( properties == &m_properties && !m_classes.empty() ) {
			for ( const auto& c : m_classes ) {
				const auto& pp = c->GetProperties();
				const auto& it2 = pp.find( key );
				if ( it2 != pp.end() ) {
					SetProperty( GSE_CALL, properties, it2->first, it2->second );
					return;
				}
			}
		}
		properties->erase( it );
		if ( properties == &m_properties ) {
			if ( m_wrapobj ) {
				m_wrapobj->Assign( key, nullptr );
			}
		}
		if ( m_is_initialized && properties == &m_properties ) {
			OnPropertyRemove( GSE_CALL, key );
		}
	}
}

void Object::SetClasses( GSE_CALLABLE, const std::vector< std::string >& names ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	if ( m_is_initialized ) {
		for ( const auto& c : m_classes ) {
			c->RemoveObject( GSE_CALL, this );
		}
		m_classes.clear();
	}
	else {
		ASSERT( m_classes.empty(), "not initialized but classes not empty" );
	}
	m_classes.reserve( names.size() );
	for ( const auto& name : names ) {
		auto* c = m_ui->GetClass( name );
		if ( !c ) {
			GSE_ERROR( gse::EC.UI_ERROR, "Class '" + name + "' does not exist" );
		}
		m_classes.push_back( c );
		if ( m_is_initialized ) {
			c->AddObject( GSE_CALL, this, m_modifiers );
		}
	}
}

void Object::SetPropertyFromClass( GSE_CALLABLE, const std::string& key, gse::Value* const value, const class_modifier_t modifier ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	const auto& def_it = m_property_defs.find( key );
	if ( def_it == m_property_defs.end() ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' does not exist" );
	}
	if ( m_manual_properties.find( key ) == m_manual_properties.end() ) {
		// check if property was set by any of previous classes with higher modifier
		for ( const auto& c : m_classes ) {
			const auto kv = c->GetProperty( key, m_modifiers );
			if ( kv.first && kv.second > modifier ) {
				return;
			}
		}
		SetProperty( GSE_CALL, &m_properties, key, value );
	}
}

void Object::UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) {
	ASSERT( !m_is_destroyed, "object is destroyed" );
	const auto& def_it = m_property_defs.find( key );
	ASSERT( def_it != m_property_defs.end(), "property def not found: " + key );

	gse::Value* v = nullptr;

	// search in manual properties
	const auto& it = m_manual_properties.find( key );
	if ( it != m_manual_properties.end() ) {
		v = it->second;
	}

	if ( !v ) {
		// search in other classes
		for ( auto it2 = m_classes.rbegin() ; it2 != m_classes.rend() ; it2++ ) {
			const auto kv = ( *it2 )->GetProperty( key, m_modifiers );
			if ( kv.first ) {
				v = kv.first;
				break;
			}
		}
	}

	if ( !v ) {
		// search in default properties
		const auto& it2 = m_default_properties.find( key );
		if ( it2 != m_default_properties.end() ) {
			v = it2->second;
		}
	}

	if ( v ) {
		SetProperty( GSE_CALL, &m_properties, key, v );
	}
	else {
		UnsetProperty( GSE_CALL, &m_properties, key );
	}
}

}
}
