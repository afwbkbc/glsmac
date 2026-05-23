#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class AnyPtr : public Value {
public:
	static const value_type_t GetType() { return VT_PTR; }
	const value_type_t ptr_type = VT_NOTHING;

	virtual Value* const Clone() override = 0;

	AnyPtr( gc::Space* const gc_space, const value_type_t type );

#define OP( _op ) virtual const bool operator _op( const Value& other ) const override = 0;
	OP( == )
	OP( < )
	OP( > )
	OP( <= )
	OP( >= )
#undef OP

};

template< typename T >
class Ptr : public AnyPtr {
public:
	Ptr( gc::Space* const gc_space, T& target );

	Value* const Clone() override;

#define OP( _op ) const bool operator _op( const Value& other ) const override;
	OP( == )
	OP( < )
	OP( > )
	OP( <= )
	OP( >= )
#undef OP

private:
	T& m_target;

	const value_type_t GetPtrType() const;

};

}
}
