#pragma once

#include "type/Type.h"

#include <memory>

namespace gse {

#define VALUE( _type, ... ) Value( std::make_shared<type::_type>( __VA_ARGS__ ) )

class Value {
public:
	Value() = delete;
	Value( const std::shared_ptr< type::Type > data );
	~Value();

	const type::Type* Get() const;

private:
	std::shared_ptr< type::Type > m_data;
};

}
