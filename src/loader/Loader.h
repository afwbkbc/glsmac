#pragma once

#include <string>

#include "base/Module.h"

using namespace std;

namespace loader {

CLASS( Loader, base::Module )

protected:
	const string& GetRoot();
};

} /* namespace loader */
