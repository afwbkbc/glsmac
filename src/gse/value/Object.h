#pragma once

#include <map>
#include <string>
#include <functional>

#include "Types.h"
#include "gse/Types.h"

namespace gse {

class Wrappable;

namespace context {
class Context;
class ChildContext;
}

namespace value {

class Object : public Value {
public:

	// internal use only for now
	typedef std::string object_class_t;

	static const type_t GetType() { return Value::T_OBJECT; }

	typedef void (wrapsetter_t)( Wrappable*, const std::string&, Value* const, GSE_CALLABLE ); // ( obj, key, value, GSE_CALL )
	Object( context::ChildContext* const ctx, object_properties_t initial_value = {}, const object_class_t object_class = "", Wrappable* wrapobj = nullptr, wrapsetter_t* wrapsetter = nullptr );
	~Object();

	Value* const Get( const object_key_t& key ) const;
	void Set( const object_key_t& key, Value* const value, GSE_CALLABLE );

	Value* const GetRef( const object_key_t& key );

	void Unlink();

	object_properties_t value;
	const object_class_t object_class;
	Wrappable* wrapobj;
	wrapsetter_t* wrapsetter;

private:
	context::ChildContext* const m_ctx;
};

}
}
