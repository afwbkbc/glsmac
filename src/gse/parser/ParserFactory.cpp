#include "ParserFactory.h"

#include "GJS.h"

#include "util/FS.h"

namespace gse {
namespace parser {

Parser* ParserFactory::GetParser( const std::string& filename, const std::string& source ) {
	Parser* parser = nullptr;
	const auto extension = util::FS::GetExtension( filename );
	if ( extension == "gjs" ) {
		NEW( parser, GJS, filename, source );
	}
	return parser;
}

}
}
