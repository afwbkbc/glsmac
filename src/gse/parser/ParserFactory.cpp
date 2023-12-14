#include "ParserFactory.h"

#include "GJS.h"

namespace gse {
namespace parser {

Parser* ParserFactory::GetParser( const std::string& extension, const std::string& source ) {
	Parser* parser = nullptr;
	if ( extension == "gjs" ) {
		NEW( parser, GJS, source );
	}
	return parser;
}

}
}
