#include "Object.h"

#include <unordered_map>

#include "Undefined.h"
#include "ObjectRef.h"
#include "Exception.h"
#include "String.h"

#include "gse/Wrappable.h"

namespace gse {
namespace type {

static Value s_undefined = VALUE( type::Undefined );

static const std::unordered_map< Object::object_class_t, std::string > s_object_class_str = {
	{
		Object::CLASS_NONE,
		""
	},
	{
		Object::CLASS_EXCEPTION,
		"#exception"
	},
	{
		Object::CLASS_COLOR,
		"#color"
	},
	{
		Object::CLASS_SYSTEM,
		"#system"
	},
	{
		Object::CLASS_STATE,
		"#state"
	},
	{
		Object::CLASS_GAME,
		"#game"
	},
	{
		Object::CLASS_TILE,
		"#tile"
	},
	{
		Object::CLASS_PLAYER,
		"#player"
	},
	{
		Object::CLASS_FACTION,
		"#faction"
	},
	{
		Object::CLASS_FACTIONS,
		"#factions"
	},
	{
		Object::CLASS_UNITDEF,
		"#unitdef"
	},
	{
		Object::CLASS_UNIT,
		"#unit"
	},
	{
		Object::CLASS_BASE,
		"#base"
	},
};
const std::string& Object::GetClassString( const object_class_t object_class ) {
	const auto& it = s_object_class_str.find( object_class );
	ASSERT_NOLOG( it != s_object_class_str.end(), "unknown/unsupported object class type: " + std::to_string( object_class ) );
	return it->second;
}

Object::Object( object_properties_t initial_value, const object_class_t object_class, Wrappable* wrapobj, wrapsetter_t* wrapsetter )
	: Type( GetType() )
	, value( initial_value )
	, object_class( object_class )
	, wrapobj( wrapobj )
	, wrapsetter( wrapsetter ) {
	if ( wrapobj ) {
		wrapobj->Link( this );
	}
}

Object::~Object() {
	if ( wrapobj ) {
		wrapobj->Unlink( this );
	}
}

const Value& Object::Get( const object_key_t& key ) const {
	const auto& it = value.find( key );
	return it == value.end()
		? s_undefined
		: it->second;
}

void Object::Set( const object_key_t& key, const Value& new_value, context::Context* ctx, const si_t& si ) {
	if ( wrapobj ) {
		if ( !wrapsetter ) {
			throw gse::Exception( EC.INVALID_ASSIGNMENT, "Property is read-only", ctx, si );
		}
		wrapsetter( wrapobj, key, new_value, ctx, si );
	}
	value.insert_or_assign( key, new_value );
}

const Value Object::GetRef( const object_key_t& key ) {
	return VALUE( ObjectRef, this, key );
}

void Object::Unlink() {
	ASSERT_NOLOG( wrapobj, "wrapobj not linked" );
	wrapobj = nullptr;
}

}
}
