#pragma once

#include <string>

#include "Object.h"

#include "gse/Exception.h"
#include "String.h"
#include "Array.h"

namespace gse {
namespace type {

class Exception : public Object {
public:

	Exception( const gse::Exception& exception )
		: Object(
		{
			{
				"type",
				VALUE( type::String, exception.class_name )
			},
			{
				"reason",
				VALUE( type::String, exception.reason )
			},
			{
				"backtrace",
				VALUE( type::Array, type::Array::elements_t{} )
			}
		}
	) {}

	//, exception( exception ) {}

	//const gse::Exception exception;
};

}
}
