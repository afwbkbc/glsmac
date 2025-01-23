#pragma once

#include <map>
#include <string>
#include <set>

#include "gse/Value.h"

namespace ui {

typedef float coord_t;

typedef std::map< std::string, gse::Value > properties_t;
typedef std::vector< std::pair< std::string, gse::Value > > ordered_properties_t;

// order is important, from lowest priority to highest
enum class_modifier_t : uint8_t {
	CM_NONE,
	CM_FOCUS,
	CM_HOVER,
	CM_ACTIVE,
};
typedef std::set< class_modifier_t > class_modifiers_t;

}
