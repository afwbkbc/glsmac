#pragma once

#include "type/Type.h"

namespace gse {

class Value {
public:
	Value() = delete;
	Value( const type::Type* data );
	~Value();

	Value operator=( const Value& other );

private:
	const type::Type* m_data;
};

}
