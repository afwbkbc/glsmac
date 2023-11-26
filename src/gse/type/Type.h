#pragma once

#include <cstdint>

namespace gse {
namespace type {

class Type {
public:
	enum type_t : uint8_t {
		T_UNDEFINED,
		T_NULL,
		T_BOOL,
		T_INT,
		T_OBJECT,
		T_CALLABLE,
	};

	const type_t type;

protected:
	Type( const type_t type )
		: type( type ) {}

};

}
}
