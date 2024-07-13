#pragma once

#include <cstdint>

namespace game {
namespace backend {
namespace slot {

typedef uint8_t player_flag_t;
static const player_flag_t PF_NONE = 0;
static const player_flag_t PF_READY = 1 << 0;
static const player_flag_t PF_MAP_DOWNLOADED = 1 << 1;
static const player_flag_t PF_GAME_INITIALIZED = 1 << 2;

}
}
}
