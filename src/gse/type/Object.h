#pragma once

#include <map>
#include <string>
#include <functional>

#include "Type.h"
#include "../Value.h"
#include "../Context.h"

namespace gse {

class Wrappable;

namespace type {

static const std::string OBJECT_CLASS_TILE = "Tile";

class Object : public Type {
public:

	// internal use only for now
	enum object_class_t {
		CLASS_NONE,
		CLASS_EXCEPTION,
		CLASS_COLOR,
		CLASS_TILE,
		CLASS_PLAYER,
		CLASS_FACTION,
		CLASS_UNITDEF,
		CLASS_UNIT,
	};
	static const std::string& GetClassString( const object_class_t object_class );

	static const type_t GetType() { return Type::T_OBJECT; }

	typedef std::string key_t; // keep it simple for now
	typedef std::map< key_t, Value > properties_t;

	typedef void (wrapsetter_t)( Wrappable*, const std::string&, const gse::Value&, gse::Context* ctx, const gse::si_t& si ); // ( obj, key, value, ctx, si )
	Object( properties_t initial_value = {}, const object_class_t object_class = CLASS_NONE, Wrappable* wrapobj = nullptr, wrapsetter_t* wrapsetter = nullptr );
	~Object();

	const Value& Get( const key_t& key ) const;
	void Set( const key_t& key, const Value& value, gse::Context* ctx, const si_t& si );

	const Value GetRef( const key_t& key );

	void Unlink();

	properties_t value;
	const object_class_t object_class;
	Wrappable* wrapobj;
	wrapsetter_t* wrapsetter;

};

}
}
