#pragma once

#include <map>
#include <string>
#include <functional>

#include "Types.h"
#include "gse/Types.h"

#include "Type.h"
#include "gse/Value.h"

namespace gse {

class Wrappable;

namespace context {
class Context;
}

namespace type {

class Object : public Type {
public:

	// internal use only for now
	enum object_class_t {
		CLASS_NONE,
		CLASS_EXCEPTION,
		CLASS_COLOR,
		CLASS_SYSTEM,
		CLASS_STATE,
		CLASS_GAME,
		CLASS_TILE,
		CLASS_PLAYER,
		CLASS_FACTION,
		CLASS_FACTIONS,
		CLASS_UNITDEF,
		CLASS_UNIT,
		CLASS_BASE,
		CLASS_BASE_POP,
	};
	static const std::string& GetClassString( const object_class_t object_class );

	static const type_t GetType() { return Type::T_OBJECT; }

	typedef void (wrapsetter_t)( Wrappable*, const std::string&, const Value&, context::Context* ctx, const si_t& si ); // ( obj, key, value, ctx, si )
	Object( object_properties_t initial_value = {}, const object_class_t object_class = CLASS_NONE, Wrappable* wrapobj = nullptr, wrapsetter_t* wrapsetter = nullptr );
	~Object();

	const Value& Get( const object_key_t& key ) const;
	void Set( const object_key_t& key, const Value& value, context::Context* ctx, const si_t& si );

	const Value GetRef( const object_key_t& key );

	void Unlink();

	object_properties_t value;
	const object_class_t object_class;
	Wrappable* wrapobj;
	wrapsetter_t* wrapsetter;

};

}
}
