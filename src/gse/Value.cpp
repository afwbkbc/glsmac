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

}