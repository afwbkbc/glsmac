#pragma once

#include <map>
#include <string>
#include <set>

namespace gse {
class Value;
};

namespace ui {

typedef float coord_t;

typedef std::map< std::string, gse::Value* > properties_t;

// order is important, from lowest priority to highest
enum class_modifier_t : uint8_t {
	CM_NONE,
	CM_FOCUS,
	CM_HOVER,
	CM_SELECTED,
	CM_HIGHLIGHT,
	CM_ACTIVE,
};
typedef std::set< class_modifier_t > class_modifiers_t;

enum geometry_handler_type_t {
	GH_ON_AREA_UPDATE,
	GH_ON_EFFECTIVE_AREA_UPDATE,
	GH_ON_CHILD_UPDATE,
};
typedef uint8_t geometry_handler_id_t;

}
