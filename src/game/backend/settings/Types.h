#pragma once

#include <cstdint>

namespace game {
namespace backend {
namespace settings {

typedef uint8_t map_config_value_t;

static constexpr map_config_value_t MAP_CONFIG_CUSTOM = 0;
static constexpr map_config_value_t MAP_CONFIG_TINY = 1;
static constexpr map_config_value_t MAP_CONFIG_SMALL = 2;
static constexpr map_config_value_t MAP_CONFIG_STANDARD = 3;
static constexpr map_config_value_t MAP_CONFIG_LARGE = 4;
static constexpr map_config_value_t MAP_CONFIG_HUGE = 5;

static constexpr map_config_value_t MAP_CONFIG_OCEAN_LOW = 1;
static constexpr map_config_value_t MAP_CONFIG_OCEAN_MEDIUM = 2;
static constexpr map_config_value_t MAP_CONFIG_OCEAN_HIGH = 3;

static constexpr map_config_value_t MAP_CONFIG_EROSIVE_STRONG = 1;
static constexpr map_config_value_t MAP_CONFIG_EROSIVE_AVERAGE = 2;
static constexpr map_config_value_t MAP_CONFIG_EROSIVE_WEAK = 3;

static constexpr map_config_value_t MAP_CONFIG_LIFEFORMS_RARE = 1;
static constexpr map_config_value_t MAP_CONFIG_LIFEFORMS_AVERAGE = 2;
static constexpr map_config_value_t MAP_CONFIG_LIFEFORMS_ABUNDANT = 3;

static constexpr map_config_value_t MAP_CONFIG_CLOUDS_SPARSE = 1;
static constexpr map_config_value_t MAP_CONFIG_CLOUDS_AVERAGE = 2;
static constexpr map_config_value_t MAP_CONFIG_CLOUDS_DENSE = 3;

}
}
}
