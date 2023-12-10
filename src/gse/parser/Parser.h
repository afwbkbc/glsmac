#pragma once

#include "base/Base.h"

#include "gse/type/Callable.h"
#include "gse/program/Program.h"

namespace gse {
namespace parser {

CLASS( Parser, base::Base )

	virtual const std::string GetParserName() const = 0;
	virtual const std::vector< std::string > GetSupportedExtensions() const = 0;

	virtual const program::Program* Parse( const std::string& extension, const std::string& source ) const = 0;

};

}
}
