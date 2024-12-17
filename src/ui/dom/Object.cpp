#include "Object.h"

#include <atomic>

namespace ui {
namespace dom {

static std::atomic< id_t > s_next_id = 0;

Object::Object( const std::string& cls, Object* const parent, const properties_t& properties )
	: m_class( "$" + cls )
	, m_id( ++s_next_id )
	, m_parent( parent )
	, m_initial_properties( properties ) {
	Property( "id", gse::type::Type::T_STRING );
}

//const gse::type::Object::object_class_t Object::WRAP_CLASS = "Object";
const gse::Value Object::Wrap( const bool dynamic ) {
	gse::type::object_properties_t properties = {};
	for ( const auto& p : m_properties ) {
		if ( p.second.value.Get()->type != gse::type::Type::T_UNDEFINED ) {
			properties.insert(
				{
					p.first,
					p.second.value
				}
			);
		}
	}
	return gse::Value(
		std::make_shared< gse::type::Object >(
			properties, m_class, this, dynamic
				? &Object::WrapSet
				: nullptr
		)
	);
}
void Object::WrapSet( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& si ) {
	ASSERT_NOLOG( wrapobj, "wrapobj not set" );
	auto* obj = (Object*)wrapobj;
	auto it = obj->m_properties.find( key );
	if ( it == obj->m_properties.end() ) {
		throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + key + "' does not exist", ctx, si );
	}
	if ( it->second.flags & PF_READONLY ) {
		throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property '" + key + "' is read-only", ctx, si );
	}
	if ( value != it->second.value ) {
		it->second.value = value;
		obj->m_changes.insert( key );
	}
}

void Object::Property( const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value, const property_flag_t flags ) {
	ASSERT_NOLOG( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already exists" );
	const auto& it = m_initial_properties.find( name );
	m_properties.insert(
		{
			name,
			{
				type,
				it == m_initial_properties.end()
					? default_value
					: it->second,
				flags
			}
		}
	);
}

void Object::Method( const std::string& name, const gse::Value& callable ) {
	ASSERT_NOLOG( callable.Get()->type == gse::type::Type::T_CALLABLE, "method is not callable: " + callable.ToString() );
	Property( name, gse::type::Type::T_CALLABLE, callable, PF_READONLY );
}

/*Object* Object::Unwrap( const gse::Value& value ) {
	const auto* valueobj = value.Get()->Deref();
	if ( !( valueobj->type == gse::type::Type::T_OBJECT ) ) { throw std::runtime_error( "can't unwrap non-object: " + valueobj->Dump() ); };
	const auto* obj = (gse::type::Object*)valueobj;
	if ( !( obj->object_class == WRAP_CLASS ) ) { throw std::runtime_error( "can't unwrap object of different class ( " + obj->object_class + " != " + WRAP_CLASS + " )" ); };
	if ( !( obj->wrapobj ) ) { throw std::runtime_error( "can't unwrap object without internal link" ); };
	return (Object*)obj->wrapobj;
}*/

}
}
