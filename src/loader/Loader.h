#pragma once

#include <string>

#include "common/Module.h"

#include "resource/Types.h"

namespace loader {

CLASS( Loader, common::Module )

protected:
	const std::string& GetFilename( const resource::resource_t res ) const;
	const std::string& GetPath( const resource::resource_t res ) const;
	const std::string& GetCustomFilename( const std::string& filename ) const;

};

}
