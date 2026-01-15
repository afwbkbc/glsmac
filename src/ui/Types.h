#pragma once

#include <map>
#include <string>
#include <set>
#include <cstdint>
#include <functional>

#include "gse/Types.h"

namespace gse {
class Value;
namespace value {
class Object;
}
}

namespace ui {

namespace dom {
class Widget;
}

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

enum widget_type_t {
	WT_NONE,
	WT_MINIMAP,
	WT_TILE_PREVIEW,
	WT_UNIT_PREVIEW,
	WT_BASE_PREVIEW,
};

typedef std::function< void( ::ui::dom::Widget* const widget ) > f_with_widget_t;
#define F_WITH_WIDGET( ... ) [ __VA_ARGS__ ]( ::ui::dom::Widget* const widget )
struct widget_data_config_entry_t {
	gse::value_type_t type;
	std::string object_class;
};
typedef std::unordered_map< std::string, widget_data_config_entry_t > widget_data_config_t;

typedef std::function< void( ::ui::dom::Widget* const widget, gse::value::Object* const data ) > f_widget_update_t;
#define F_WIDGET_UPDATE( ... ) [ __VA_ARGS__ ]( ::ui::dom::Widget* const widget, gse::value::Object* const data )

typedef uint64_t object_id_t;

}
