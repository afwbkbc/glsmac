#include "Exception.h"

#include "String.h"
#include "Array.h"

namespace gse {
namespace value {

Exception::Exception( GSE_CALLABLE, const gse::Exception& exception, const std::vector< std::string >& stacktrace )
	: Object(
	GSE_CALL,
	{
		{
			"type",
			VALUEEXT( value::String, gc_space, exception.class_name )
		},
		{
			"reason",
			VALUEEXT( value::String, gc_space, exception.reason )
		},
	},
	"Exception"
) {
	array_elements_t bt = {};
	for ( const auto& it : stacktrace ) {
		bt.push_back( VALUEEXT( value::String, m_gc_space, it ) );
	}
	value.insert_or_assign( "stacktrace", VALUE( value::Array, , bt ) );
}

}
}
