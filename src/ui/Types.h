#pragma once

#include <map>
#include <string>

#include "gse/Value.h"

namespace ui {

typedef float coord_t;

typedef std::map< std::string, gse::Value > properties_t;
typedef std::vector< std::pair< std::string, gse::Value > > ordered_properties_t;

}
