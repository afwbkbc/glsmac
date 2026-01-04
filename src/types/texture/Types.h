#pragma once

#include <cstdint>

#include "types/Color.h"

namespace types {
namespace texture {

// bitflags
typedef uint32_t add_flag_t;
static constexpr add_flag_t AM_DEFAULT = 0;
static constexpr add_flag_t AM_MERGE = 1 << 0; // copy only non-transparent pixels
// round one or several corners
static constexpr add_flag_t AM_ROUND_LEFT = 1 << 1;
static constexpr add_flag_t AM_ROUND_TOP = 1 << 2;
static constexpr add_flag_t AM_ROUND_RIGHT = 1 << 3;
static constexpr add_flag_t AM_ROUND_BOTTOM = 1 << 4;
// invert texture while copying, upside down or left-right
static constexpr add_flag_t AM_INVERT = 1 << 5; // add unneeded pixels instead of needed
static constexpr add_flag_t AM_MIRROR_X = 1 << 6; // mirrors source by x
static constexpr add_flag_t AM_MIRROR_Y = 1 << 7; // mirrors source by y
static constexpr add_flag_t AM_RANDOM_MIRROR_X = 1 << 8; // sometimes mirror by x
static constexpr add_flag_t AM_RANDOM_MIRROR_Y = 1 << 9; // sometimes mirror by x
// copy as gradient, alpha will decrease towards center
static constexpr add_flag_t AM_GRADIENT_LEFT = 1 << 10;
static constexpr add_flag_t AM_GRADIENT_TOP = 1 << 11;
static constexpr add_flag_t AM_GRADIENT_RIGHT = 1 << 12;
static constexpr add_flag_t AM_GRADIENT_BOTTOM = 1 << 13;
static constexpr add_flag_t AM_GRADIENT_TIGHTER = 1 << 14; // gradient range is twice smaller
// shift one part before other at random distance
static constexpr add_flag_t AM_RANDOM_SHIFT_X = 1 << 15;
static constexpr add_flag_t AM_RANDOM_SHIFT_Y = 1 << 16;
// generate curved borders by perlin noise
static constexpr add_flag_t AM_PERLIN_LEFT = 1 << 17;
static constexpr add_flag_t AM_PERLIN_TOP = 1 << 18;
static constexpr add_flag_t AM_PERLIN_RIGHT = 1 << 19;
static constexpr add_flag_t AM_PERLIN_BOTTOM = 1 << 20;
// use those if perlin pattern shouldn't go till the end (it will gradually reduce towards last 1/3)
static constexpr add_flag_t AM_PERLIN_CUT_LEFT = 1 << 21;
static constexpr add_flag_t AM_PERLIN_CUT_TOP = 1 << 22;
static constexpr add_flag_t AM_PERLIN_CUT_RIGHT = 1 << 23;
static constexpr add_flag_t AM_PERLIN_CUT_BOTTOM = 1 << 24;
// adds border to perlin pattern
static constexpr add_flag_t AM_COASTLINE_BORDER = 1 << 25;
// randomly stretch texture during copy
static constexpr add_flag_t AM_RANDOM_STRETCH = 1 << 26; // just stretch up
static constexpr add_flag_t AM_RANDOM_STRETCH_SHRINK = 1 << 27; // texture can stretch down too
static constexpr add_flag_t AM_RANDOM_STRETCH_SHIFT = 1 << 28; // also start x and y from random positions
static constexpr add_flag_t AM_RANDOM_STRETCH_SHUFFLE = 1 << 29; // shuffle more. also implies all other stretch flags
// other
static constexpr add_flag_t AM_KEEP_TRANSPARENCY = 1 << 30; // copy only where destination is not transparent

typedef uint8_t rotate_t;
static constexpr rotate_t ROTATE_0 = 0;
static constexpr rotate_t ROTATE_90 = 1;
static constexpr rotate_t ROTATE_180 = 2;
static constexpr rotate_t ROTATE_270 = 3;

typedef std::unordered_map< types::Color::rgba_t, types::Color::rgba_t > repaint_rules_t;

enum texture_flag_t {
	TF_NONE = 0,
	TF_MIPMAPS = 1 << 0,
};

}
}
