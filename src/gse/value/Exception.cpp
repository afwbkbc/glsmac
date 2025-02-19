#include "Exception.h"

#include "String.h"
#include "Array.h"

namespace gse {
namespace value {

Exception::Exception( const gse::Exception& exception, const std::vector< std::string >& stacktrace )
	: Object(
	nullptr,
	{
		{
			"type",
			VALUE( value::String, exception.class_name )
		},
		{
			"reason",
			VALUE( value::String, exception.reason )
		},
	},
	"Exception"
) {
	array_elements_t bt = {};
	for ( const auto& it : stacktrace ) {
		bt.push_back( VALUE( value::String, it ) );
	}
	value.insert_or_assign( "stacktrace", VALUE( value::Array, bt ) );
}

}
}
