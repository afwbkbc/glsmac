#include "Object.h"

#include <atomic>

#include "Container.h"
#include "util/String.h"
#include "scene/actor/Actor.h"
#include "ui/UI.h"
#include "scene/Scene.h"

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
}

Object::~Object() {
	for ( const auto& actor : m_actors ) {
		m_ui->m_scene->RemoveActor( actor );
		delete actor;
	}
};

const gse::Value Object::Wrap( const bool dynamic ) {
	gse::type::object_properties_t properties = {};
	for ( const auto& p : m_properties ) {
		properties.insert(
			{
				p.first,
				p.second.value
			}
		);
	}
	return gse::Value(
		std::make_shared< gse::type::Object >(
			properties, m_tag, this, dynamic
				? &Object::WrapSetStatic
				: nullptr
		)
	);
}

void Object::WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si ) {
	auto it = m_properties.find( key );
	if ( it == m_properties.end() ) {
		throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + key + "' does not exist", ctx, si );
	}
	if ( it->second.flags & PF_READONLY ) {
		throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + key + "' is read-only", ctx, si );
	}
	if ( value != it->second.value ) {
		if ( value.Get()->type != it->second.type ) {
			throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + key + "' is expected to be " + value.Get()->GetTypeString( it->second.type ) + ", got " + value.GetTypeString() + ": " + value.ToString(), ctx, si );
		}
		it->second.value = value;
		m_changes.insert( key );
		if ( it->second.f_on_set ) {
			it->second.f_on_set( ctx, si, value );
		}
	}
}

void Object::WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si ) {
	ASSERT_NOLOG( wrapobj, "wrapobj not set" );
	( (Object*)wrapobj )->WrapSet( key, value, ctx, si );
}

geometry::Geometry* const Object::GetGeometry() const {
	return nullptr;
}

void Object::Actor( scene::actor::Actor* actor ) {
	if ( m_parent ) {
		actor->SetCacheParent( m_parent->m_cache );
	}
	m_ui->m_scene->AddActor( actor );
	m_actors.push_back( actor );
}

void Object::Property( GSE_CALLABLE, const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value, const property_flag_t flags, const f_on_set_t& f_on_set ) {
	ASSERT_NOLOG( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already exists" );
	const auto& it = m_initial_properties.find( name );
	const gse::Value* value = &default_value;
	if ( it != m_initial_properties.end() ) {
		if ( it->second.Get()->type != type ) {
			throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + name + "' is expected to be " + default_value.Get()->GetTypeString( type ) + ", got " + it->second.GetTypeString() + ": " + it->second.ToString(), ctx, call_si );
		}
		value = &it->second;
	}
	const gse::Value& v = it == m_initial_properties.end()
		? default_value
		: *value;
	m_properties.insert(
		{
			name,
			{
				type,
				v,
				flags,
				f_on_set
			}
		}
	);
	if ( v.Get()->type != gse::type::Type::T_UNDEFINED ) {
		if ( f_on_set ) {
			f_on_set( ctx, call_si, v );
		}
	}
}

void Object::Method( GSE_CALLABLE, const std::string& name, const gse::Value& callable ) {
	ASSERT_NOLOG( callable.Get()->type == gse::type::Type::T_CALLABLE, "method is not callable: " + callable.ToString() );
	Property( ctx, call_si, name, gse::type::Type::T_CALLABLE, callable, PF_READONLY );
}

void Object::ParseColor( GSE_CALLABLE, const std::string& str, types::Color& color ) const {
	if ( !util::String::ParseColor( str, color ) ) {
		throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property 'color' has invalid color format. Expected hex values ( #RRGGBB or #RRGGBBAA ), got: " + str, ctx, call_si );
	}
}

/*Object* Object::Unwrap( const gse::Value& value ) {
	const auto* valueobj = value.Get()->Deref();
	if ( !( valueobj->type == gse::type::Type::T_OBJECT ) ) { throw std::runtime_error( "can't unwrap non-object: " + valueobj->Dump() ); };
	const auto* obj = (gse::type::Object*)valueobj;
	if ( !( obj->object_class == WRAP_CLASS ) ) { throw std::runtime_error( "can't unwrap object of different class ( " + obj->object_class + " != " + WRAP_CLASS + " )" ); };
	if ( !( obj->wrapobj ) ) { throw std::runtime_error( "can't unwrap object without internal link" ); };
	return (Object*)obj->wrapobj;
}*/

void Object::Validate( GSE_CALLABLE ) const {
	for ( const auto& p : m_initial_properties ) {
		if ( m_properties.find( p.first ) == m_properties.end() ) {
			throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + p.first + "' does not exist", ctx, call_si );
		}
	}
}

}
}
