#include "Object.h"

namespace ui {
namespace dom {

Object::Object() {
	Property( "key1", gse::type::Type::T_STRING, VALUE( gse::type::String, "value1" ) );
	Property( "key2", gse::type::Type::T_STRING, VALUE( gse::type::String, "value2" ) );
	Property( "key3", gse::type::Type::T_STRING, VALUE( gse::type::String, "value3" ) );
}

const gse::type::Object::object_class_t Object::WRAP_CLASS = "Object";
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
			properties, WRAP_CLASS, this, dynamic
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
	if ( value != it->second.value ) {
		it->second.value = value;
		obj->m_changes.insert( key );
	}
}

void Object::Property( const std::string& name, const gse::type::Type::type_t& type, const gse::Value& default_value ) {
	ASSERT_NOLOG( m_properties.find( name ) == m_properties.end(), "property '" + name + "' already exists" );
	m_properties.insert(
		{
			name,
			{
				type,
				default_value
			}
		}
	);
}

Object* Object::Unwrap( const gse::Value& value ) {
	const auto* valueobj = value.Get()->Deref();
	if ( !( valueobj->type == gse::type::Type::T_OBJECT ) ) { throw std::runtime_error( "can't unwrap non-object: " + valueobj->Dump() ); };
	const auto* obj = (gse::type::Object*)valueobj;
	if ( !( obj->object_class == WRAP_CLASS ) ) { throw std::runtime_error( "can't unwrap object of different class ( " + obj->object_class + " != " + WRAP_CLASS + " )" ); };
	if ( !( obj->wrapobj ) ) { throw std::runtime_error( "can't unwrap object without internal link" ); };
	return (Object*)obj->wrapobj;
}

}
}
