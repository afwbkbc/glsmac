#pragma once

#include <string>

#include "base/Module.h"

namespace loader {

CLASS( Loader, base::Module )

protected:
	const std::string& GetRoot();
};

} /* namespace loader */
