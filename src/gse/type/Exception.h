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

	Exception( const gse::Exception& exception, const std::vector< std::string >& backtrace )
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
		},
		Object::CLASS_EXCEPTION
	) {
		type::Array::elements_t bt = {};
		for ( const auto& it : backtrace ) {
			bt.push_back( VALUE( type::String, it ) );
		}
		value.insert_or_assign( "backtrace", VALUE( type::Array, bt ) );
	}

};

}
}
