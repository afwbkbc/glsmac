#pragma once

#ifdef _WIN32

#include <string>
#include <vector>

namespace config {

const std::vector< std::string > GetWindowsSMACInstallPaths();

}

#endif
