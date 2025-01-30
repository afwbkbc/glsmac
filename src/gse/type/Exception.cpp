#include "Exception.h"

#include "String.h"
#include "Array.h"

namespace gse {
namespace type {

Exception::Exception( const gse::Exception& exception, const std::vector< std::string >& stacktrace )
	: Object(
	nullptr,
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
	"Exception"
) {
	array_elements_t bt = {};
	for ( const auto& it : stacktrace ) {
		bt.push_back( VALUE( type::String, it ) );
	}
	value.insert_or_assign( "stacktrace", VALUE( type::Array, bt ) );
}

}
}
