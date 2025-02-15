#pragma once

#include <cstdint>
#include <functional>

#include "types/Color.h"

namespace ui_legacy {

namespace theme {
class Style;
}

// TODO: attribute grouping/inheritance
enum attribute_type_t : uint8_t {
	A_NONE,

	// positioning
	A_ALIGN,
	A_WIDTH,
	A_HEIGHT,
	A_LEFT,
	A_TOP,
	A_RIGHT,
	A_BOTTOM,
	A_PADDING,
	A_Z_INDEX,

	// textures
	A_TEXTURE,
	A_SIZE_FROM_TEXTURE,
	A_STRETCH_TEXTURE,
	A_KEEP_TEXTURE_ASPECT_RATIO,

	// TODO: combine into multitexture?
	A_TEXTURE_BACK, // for things like panel background, scrollbar background
	A_TEXTURE_CENTER, // for things like scrollbar slider

	// borders and things like slider up button, slider down button
	A_BORDER_SIZE,
	A_TEXTURE_LEFT,
	A_TEXTURE_TOP,
	A_TEXTURE_RIGHT,
	A_TEXTURE_BOTTOM,
	A_TEXTURE_LEFTTOP,
	A_TEXTURE_RIGHTTOP,
	A_TEXTURE_RIGHTBOTTOM,
	A_TEXTURE_LEFTBOTTOM,

	// text
	A_FONT,
	A_TEXT_ALIGN,
	A_TEXT_LEFT,
	A_TEXT_COLOR,
	A_HINT_COLOR,

	// sounds
	A_SOUND,
	A_SOUND_VOLUME,
	A_SOUND_AUTOPLAY, // start playing when element is created
	A_SOUND_AUTOSTOP, // stop playing when element is destroyed
	A_SOUND_REPEATABLE,
	A_SOUND_START_DELAY,
	A_BUTTON_CLICK_SOUND,
	A_BUTTON_MOVE_SOUND,

	// headers
	A_HEADER_HEIGHT,
	A_HEADER_TEXTALIGN,
	A_HEADER_TEXTCOLOR,
	A_HEADER_FONT,
	// header borders
	A_HEADER_TEXTURE_BACK,
	A_HEADER_TEXTURE_LEFT,
	A_HEADER_TEXTURE_TOP,
	A_HEADER_TEXTURE_RIGHT,
	A_HEADER_TEXTURE_BOTTOM,

	// lists etc
	A_ITEM_WIDTH,
	A_ITEM_HEIGHT,
	A_ITEM_MARGIN,
	A_ITEM_TEXTURE,
	A_ITEM_ICON_WIDTH,
	A_ITEM_ICON_TEXTURE,

	// must be last
	ATTRIBUTE_TYPE_MAX
};

struct attribute_t {
	bool is_set = false;
	union {
		float scalar;
		types::Color::color_t color;
		const void* ptr;
	} value = {};
};
typedef attribute_t attributes_t[ATTRIBUTE_TYPE_MAX];

// bit flags
typedef uint8_t modifier_t;
static constexpr modifier_t M_NONE = 0;
static constexpr modifier_t M_HOVER = 1 << 0;
static constexpr modifier_t M_ACTIVE = 1 << 1;
static constexpr modifier_t M_SELECTED = 1 << 2; // also means "focused" (for inputs)
static constexpr modifier_t M_HIGHLIGHT = 1 << 3;
static constexpr modifier_t MODIFIER_MAX = 1 << 4;

typedef uint8_t alignment_t;
const static alignment_t ALIGN_NONE = 0;
const static alignment_t ALIGN_LEFT = 1 << 0;
const static alignment_t ALIGN_RIGHT = 1 << 1;
const static alignment_t ALIGN_HCENTER = ALIGN_LEFT | ALIGN_RIGHT;
const static alignment_t ALIGN_TOP = 1 << 2;
const static alignment_t ALIGN_BOTTOM = 1 << 3;
const static alignment_t ALIGN_VCENTER = ALIGN_TOP | ALIGN_BOTTOM;
const static alignment_t ALIGN_CENTER = ALIGN_HCENTER | ALIGN_VCENTER;

enum overflow_t {
	OVERFLOW_VISIBLE, // Allow children to be rendered outside of this object (default)
	OVERFLOW_VISIBLE_ALWAYS, // Render outside of object and override OVERFLOW_HIDDEN of parent
	OVERFLOW_HIDDEN, // Hide parts of children that are outside of this object
	OVERFLOW_GROW // Grow automatically when children are added
};

typedef std::vector< std::string > includes_t;
typedef std::function< void( theme::Style* ) > style_handler_t;

// loadtexture flags
static constexpr uint8_t LT_NONE = 0;
static constexpr uint8_t LT_ROTATE = 1;
static constexpr uint8_t LT_FLIPV = 2;
static constexpr uint8_t LT_FLIPH = 4;
static constexpr uint8_t LT_TILED = 8;
static constexpr uint8_t LT_ALPHA = 16;
static constexpr uint8_t LT_CONTRAST = 32;

}
