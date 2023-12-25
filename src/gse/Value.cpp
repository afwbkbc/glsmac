#include "Value.h"

#include "base/Base.h"

#include "type/Null.h"

#include "type/Bool.h"

namespace gse {

Value::Value( const std::shared_ptr< type::Type > data )
	: m_data( data ) {
	// nothing
}

Value::~Value() {
	// nothing
}

const type::Type* Value::Get() const {
	return m_data.get();
}

const std::string Value::ToString() const {
	return m_data->ToString();
}

#define OP( _op ) \
const bool Value::operator _op( const Value& other ) const { \
    return *m_data _op *other.m_data; \
}
OP( == )
OP( != )
OP( < )
OP( > )
OP( <= )
OP( >= )
#undef OP

}
