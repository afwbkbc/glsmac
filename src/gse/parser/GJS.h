#pragma once

#include "Parser.h"

namespace gse {
namespace parser {

CLASS( GJS, Parser )

	const std::string GetParserName() const override { return "GJS"; }
	const std::vector< std::string > GetSupportedExtensions() const override {
		return { "gjs" };
	}

	const program::Program* Parse( const std::string& extension, const std::string& source ) const override;

};

}
}
