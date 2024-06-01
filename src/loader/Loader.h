#pragma once

#include <string>

#include "base/Module.h"

#include "resource/Types.h"

namespace loader {

CLASS( Loader, base::Module )

protected:
	const std::string& GetFilename( const resource::resource_t res ) const;
	const std::string& GetCustomFilename( const std::string& filename ) const;

};

}
