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
#include "gse/type/Bool.h"

#include "Object.colormap.cpp"

namespace ui {
namespace dom {

static std::atomic< id_t > s_next_id = 0;

Object::Object( DOM_ARGS_T )
	: m_ui( ui )
	, m_tag( "$" + tag )
	, m_id( ++s_next_id )
	, m_parent( parent )
	, m_initial_properties( properties ) {
	Property( GSE_CALL, "id", gse::type::Type::T_STRING );

	Property(
		GSE_CALL, "class", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& value ) {
			const auto names = util::String::Split( ( (gse::type::String*)value.Get() )->value, ' ' );
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
		ASSERT_NOLOG( m_properties.at("class").Get()->type == gse::type::Type::T_STRING, "class is not string" );
		auto names = util::String::Split( ((gse::type::String*)m_properties.at("class").Get())->value, ' ' );
		if ( std::find( names.begin(), names.end(), name ) == names.end() ) {
			names.push_back( name );
			UpdateProperty( "class", VALUE( gse::type::String, util::String::Join( names, ' ' ) ) );
			SetClasses( GSE_CALL, names );
		}
		return VALUE( gse::type::Undefined );
	} ) );
	Method( GSE_CALL, "removeclass", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( name, 0, String );
		ASSERT_NOLOG( m_properties.at( "class" ).Get()->type == gse::type::Type::T_STRING, "class is not string" );
		const auto names = util::String::Split( ( (gse::type::String*)m_properties.at( "class" ).Get() )->value, ' ' );
		if ( std::find( names.begin(), names.end(), name ) != names.end() ) {
			std::vector< std::string > names_new = {};
			names_new.reserve( names.size() - 1 );
			for ( const auto& n : names ) {
				if ( n != name ) {
					names_new.push_back( n );
				}
			}
			UpdateProperty( "class", VALUE( gse::type::String, util::String::Join( names_new, ' ' ) ) );
			SetClasses( GSE_CALL, names_new );
		}
		return VALUE( gse::type::Undefined );
	} ) );
	Method( GSE_CALL, "remove", NATIVE_CALL( this ) {
		ASSERT_NOLOG( m_parent, "remove without parent" );
		m_parent->RemoveChild( GSE_CALL, this );
		return VALUE( gse::type::Undefined );
	} ) );
}

Object::~Object() {
	Hide();
	if ( m_is_iterable_set ) {
		m_ui->RemoveIterable( this );
	}
	for ( const auto& actor : m_actors ) {
		m_ui->m_scene->RemoveActor( actor );
		delete actor;
	}
};

const gse::Value Object::Wrap( const bool dynamic ) {
	if ( !m_wrapobj ) {
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
		m_wrapobj = std::make_shared< gse::type::Object >(
			nullptr, properties, m_tag, this, dynamic
				? &Object::WrapSetStatic
				: nullptr
		);
	}
	return gse::Value( m_wrapobj );
}

void Object::WrapSet( const std::string& key, const gse::Value& value, GSE_CALLABLE ) {
	auto def_it = m_property_defs.find( key );
	if ( def_it == m_property_defs.end() ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' does not exist" );
	}
	if ( def_it->second.flags & PF_READONLY ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' is read-only" );
	}
	if ( value.Get()->type != gse::type::Type::T_UNDEFINED ) {
		SetProperty( GSE_CALL, &m_manual_properties, key, value );
		SetProperty( GSE_CALL, &m_properties, key, value );
	}
	else {
		UnsetProperty( GSE_CALL, &m_manual_properties, key );
		UnsetProperty( GSE_CALL, &m_properties, key );
	}
}

void Object::WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, GSE_CALLABLE ) {
	ASSERT_NOLOG( wrapobj, "wrapobj not set" );
	( (Object*)wrapobj )->WrapSet( key, value, GSE_CALL );
}

geometry::Geometry* const Object::GetGeometry() const {
	return nullptr;
}

const bool Object::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
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
	SetClasses( GSE_CALL, {} );
	delete this;
}

void Object::Show() {
	if ( !m_is_visible ) {
		for ( const auto& actor : m_actors ) {
			actor->Show();
		}
		m_is_visible = true;
	}
}

void Object::Hide() {
	if ( m_is_visible ) {
		for ( const auto& actor : m_actors ) {
			actor->Hide();
		}
		m_is_visible = false;
	}
}

const bool Object::IsEventRelevant( const input::Event& event ) const {
	return
		m_is_visible &&
		m_supported_events.find( event.type ) != m_supported_events.end()
	;
}

const bool Object::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	ASSERT_NOLOG( IsEventRelevant( event ), "event irrelevant" );
	const auto& event_type = event.GetTypeStr();
	if ( HasHandlers( event_type ) ) {
		gse::type::object_properties_t event_data = {};
		SerializeEvent( event, event_data );
		const auto result = Trigger( GSE_CALL, event_type, event_data, gse::type::Type::T_BOOL );
		return ( (gse::type::Bool*)result.Get() )->value;
	}
	return false;
}

void Object::UpdateProperty( const std::string& k, const gse::Value& v ) {
	m_properties.insert_or_assign( k, v );
	if ( m_wrapobj ) {
		m_wrapobj->value.insert_or_assign( k, v );
	}
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
		SetProperty( GSE_CALL, &m_manual_properties, name, it->second );
	}
	if ( v.Get()->type != gse::type::Type::T_UNDEFINED ) {
		SetProperty( GSE_CALL, &m_properties, name, v );
	}
}

void Object::Method( GSE_CALLABLE, const std::string& name, const gse::Value& callable ) {
	ASSERT_NOLOG( callable.Get()->type == gse::type::Type::T_CALLABLE, "method is not callable: " + callable.ToString() );
	Property( GSE_CALL, name, gse::type::Type::T_CALLABLE, callable, PF_READONLY );
}

void Object::Events( const std::unordered_set< input::event_type_t >& events ) {
	for ( const auto& event : events ) {
		ASSERT_NOLOG( m_supported_events.find( event ) == m_supported_events.end(), "event already handled: " + std::to_string( event ) );
		m_supported_events.insert( event );
	}
}

void Object::Iterable( const std::function< void() >& f ) {
	ASSERT_NOLOG( !m_is_iterable_set, "iterable already set" );
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
		def->second.f_on_set( GSE_CALL, value );
	}
}

void Object::OnPropertyRemove( GSE_CALLABLE, const std::string& key ) const {
	const auto& def = m_property_defs.find( key );
	ASSERT_NOLOG( def != m_property_defs.end(), "property def not found" );
	if ( def->second.f_on_unset ) {
		def->second.f_on_unset( GSE_CALL );
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

void Object::AddModifier( GSE_CALLABLE, const class_modifier_t modifier ) {
	if ( m_modifiers.find( modifier ) == m_modifiers.end() ) {
		m_modifiers.insert( modifier );
		for ( const auto& c : m_classes ) {
			c->AddObjectModifier( GSE_CALL, this, modifier );
		}
	}
}

void Object::RemoveModifier( GSE_CALLABLE , const class_modifier_t modifier ) {
	if ( m_modifiers.find( modifier ) != m_modifiers.end() ) {
		m_modifiers.erase( modifier );
		for ( const auto& c : m_classes ) {
			c->RemoveObjectModifier( GSE_CALL, this, modifier );
		}
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

void Object::SetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key, const gse::Value& value ) {
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
		if ( it->second != value ) {
			it->second = value;
			if ( m_is_initialized && properties == &m_properties ) {
				OnPropertyChange( GSE_CALL, key, value );
			}
		}
	}
}

void Object::UnsetProperty( GSE_CALLABLE, properties_t* const properties, const std::string& key ) {
	const auto& it = properties->find( key );
	if ( it != properties->end() && it->second.Get()->type != gse::type::Type::T_UNDEFINED ) {
		if ( properties == &m_properties && !m_classes.empty() ) {
			for ( const auto& c : m_classes ) {
				const auto& it2 = c->GetProperties().find( key );
				if ( it2 != c->GetProperties().end() ) {
					SetProperty( GSE_CALL, properties, it2->first, it2->second );
					return;
				}
			}
		}
		properties->erase( it );
		if ( properties == &m_properties ) {
			if ( m_wrapobj ) {
				m_wrapobj->value.erase( key );
			}
		}
		if ( m_is_initialized && properties == &m_properties ) {
			OnPropertyRemove( GSE_CALL, key );
		}
	}
}

void Object::SetClasses( GSE_CALLABLE, const std::vector< std::string >& names ) {
	if ( m_is_initialized ) {
		for ( const auto& c : m_classes ) {
			c->RemoveObject( GSE_CALL, this );
		}
		m_classes.clear();
	}
	else {
		ASSERT_NOLOG( m_classes.empty(), "not initialized but classes not empty" );
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

void Object::SetPropertyFromClass( GSE_CALLABLE, const std::string& key, const gse::Value& value, const class_modifier_t modifier ) {
	const auto& def_it = m_property_defs.find( key );
	if ( def_it == m_property_defs.end() ) {
		GSE_ERROR( gse::EC.UI_ERROR, "Property '" + key + "' does not exist" );
	}
	if ( m_manual_properties.find( key ) == m_manual_properties.end() ) {
		// check if property was set by any of previous classes with higher modifier
		for ( const auto& c : m_classes ) {
			const auto kv = c->GetProperty( key, m_modifiers );
			if ( kv.first.Get()->type != gse::type::Type::T_UNDEFINED && kv.second > modifier ) {
				return;
			}
		}
		SetProperty( GSE_CALL, &m_properties, key, value );
	}
}

void Object::UnsetPropertyFromClass( GSE_CALLABLE, const std::string& key ) {
	const auto& def_it = m_property_defs.find( key );
	ASSERT_NOLOG( def_it != m_property_defs.end(), "property def not found: " + key );

	auto v = VALUE( gse::type::Undefined );

	// search in manual properties
	const auto& it = m_manual_properties.find( key );
	if ( it != m_manual_properties.end() ) {
		v = it->second;
	}

	if ( v.Get()->type == gse::type::Type::T_UNDEFINED ) {
		// search in other classes
		for ( auto it = m_classes.rbegin() ; it != m_classes.rend() ; it++ ) {
			const auto kv = ( *it )->GetProperty( key, m_modifiers );
			v = kv.first;
			if ( v.Get()->type != gse::type::Type::T_UNDEFINED ) {
				break;
			}
		}
	}

	if ( v.Get()->type == gse::type::Type::T_UNDEFINED ) {
		// search in default properties
		const auto& it = m_default_properties.find( key );
		if ( it != m_default_properties.end() ) {
			v = it->second;
		}
	}

	if ( v.Get()->type != gse::type::Type::T_UNDEFINED ) {
		SetProperty( GSE_CALL, &m_properties, key, v );
	}
	else {
		UnsetProperty( GSE_CALL, &m_properties, key );
	}
}

}
}
