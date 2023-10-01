#pragma once

#include <string>

#include "base/Module.h"

#include "util/Random.h"
#include "types/Vec2.h"

#include "game/Settings.h"

namespace config {

CLASS( Config, base::Module )
	Config( const int argc, const char* argv[] );

	enum launch_flag_t : uint8_t {
		LF_NONE = 0,
		LF_BENCHMARK = 1 << 0,
		LF_NOSOUND = 1 << 1,
		LF_SKIPINTRO = 1 << 2,
		LF_WINDOWED = 1 << 3,
		LF_WINDOW_SIZE = 1 << 4
	};

#ifdef DEBUG
	enum debug_flag_t : uint16_t {
		DF_NONE = 0,
		DF_GDB = 1 << 0,
		DF_MEMORYDEBUG = 1 << 1,
		DF_QUICKSTART = 1 << 2,
		DF_QUICKSTART_SEED = 1 << 3,
		DF_QUICKSTART_MAP_DUMP = 1 << 4,
		DF_QUICKSTART_MAP_FILE = 1 << 5,
		DF_QUICKSTART_MAP_SIZE = 1 << 6,
		DF_QUICKSTART_MAP_OCEAN = 1 << 7,
		DF_QUICKSTART_MAP_EROSIVE = 1 << 8,
		DF_QUICKSTART_MAP_LIFEFORMS = 1 << 9,
		DF_QUICKSTART_MAP_CLOUDS = 1 << 10
	};
#endif

	const std::string& GetSMACPath() const;

	const bool HasLaunchFlag( const launch_flag_t flag ) const;
	const types::Vec2< size_t >& GetWindowSize() const;

#ifdef DEBUG

	const bool HasDebugFlag( const debug_flag_t flag ) const;
	const util::Random::state_t& GetQuickstartSeed() const;
	const std::string& GetQuickstartMapDump() const;
	const std::string& GetQuickstartMapFile() const;
	const types::Vec2< size_t >& GetQuickstartMapSize() const;
	const game::MapSettings::parameter_t GetQuickstartMapOcean() const;
	const game::MapSettings::parameter_t GetQuickstartMapErosive() const;
	const game::MapSettings::parameter_t GetQuickstartMapLifeforms() const;
	const game::MapSettings::parameter_t GetQuickstartMapClouds() const;

#endif

private:
	std::string m_smac_path = "";

	uint8_t m_launch_flags = LF_NONE;
	types::Vec2< size_t > m_window_size = {};

#ifdef DEBUG

	uint16_t m_debug_flags = DF_NONE;
	util::Random::state_t m_quickstart_seed = {};
	std::string m_quickstart_mapdump = "";
	std::string m_quickstart_mapfile = "";
	types::Vec2< size_t > m_quickstart_mapsize = {};
	game::MapSettings::parameter_t m_quickstart_map_ocean = game::MapSettings::MAP_OCEAN_MEDIUM;
	game::MapSettings::parameter_t m_quickstart_map_erosive = game::MapSettings::MAP_EROSIVE_AVERAGE;
	game::MapSettings::parameter_t m_quickstart_map_lifeforms = game::MapSettings::MAP_LIFEFORMS_AVERAGE;
	game::MapSettings::parameter_t m_quickstart_map_clouds = game::MapSettings::MAP_CLOUDS_AVERAGE;

#endif
};

} /* namespace config */

