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
class ChildContext;
}

namespace type {

class Object : public Type {
public:

	// internal use only for now
	typedef std::string object_class_t;

	static const type_t GetType() { return Type::T_OBJECT; }

	typedef void (wrapsetter_t)( Wrappable*, const std::string&, const Value&, context::Context* ctx, const si_t& si ); // ( obj, key, value, ctx, si )
	Object( context::ChildContext* const ctx, object_properties_t initial_value = {}, const object_class_t object_class = "", Wrappable* wrapobj = nullptr, wrapsetter_t* wrapsetter = nullptr );
	~Object();

	const Value& Get( const object_key_t& key ) const;
	void Set( const object_key_t& key, const Value& value, context::Context* ctx, const si_t& si );

	const Value GetRef( const object_key_t& key );

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
