#include "Value.h"

#include "base/Base.h"

#include "type/Null.h"

namespace gse {

Value::Value( const type::Type* data )
	: m_data( data ) {

}

Value::~Value() {
	delete m_data;
}

Value Value::operator=( const Value& other ) {
	// TODO
	return Value( new type::Null() );
}

}