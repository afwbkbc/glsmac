#pragma once

#include "base/Module.h"

#include <string>

using namespace std;

namespace loader {

CLASS( Loader, base::Module )

protected:
	const string& GetRoot();
};

} /* namespace loader */
