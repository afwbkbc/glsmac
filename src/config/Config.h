#pragma once

#include <string>

#include "common/Module.h"

#include "util/random/Types.h"
#include "game/settings/Types.h"
#include "types/Vec2.h"

namespace util {
class ArgParser;
}

namespace config {

CLASS( Config, common::Module )
	Config( const int argc, const char* argv[] );
	~Config();

	void Init();

	enum launch_flag_t : uint8_t {
		LF_NONE = 0,
		LF_BENCHMARK = 1 << 0,
		LF_SHOWFPS = 1 << 1,
		LF_NOSOUND = 1 << 2,
		LF_SKIPINTRO = 1 << 3,
		LF_WINDOWED = 1 << 4,
		LF_WINDOW_SIZE = 1 << 5
	};

#ifdef DEBUG
	enum debug_flag_t : uint32_t {
		DF_NONE = 0,
		DF_GDB = 1 << 0,
		DF_MAPDUMP = 1 << 1,
		DF_MEMORYDEBUG = 1 << 2,
		DF_QUICKSTART = 1 << 3,
		DF_QUICKSTART_SEED = 1 << 4,
		DF_QUICKSTART_MAP_DUMP = 1 << 5,
		DF_QUICKSTART_MAP_FILE = 1 << 6,
		DF_QUICKSTART_MAP_SIZE = 1 << 7,
		DF_QUICKSTART_MAP_OCEAN = 1 << 8,
		DF_QUICKSTART_MAP_EROSIVE = 1 << 9,
		DF_QUICKSTART_MAP_LIFEFORMS = 1 << 10,
		DF_QUICKSTART_MAP_CLOUDS = 1 << 11,
		DF_QUIET = 1 << 12,
		DF_GSE_ONLY = 1 << 13,
		DF_GSE_TESTS = 1 << 14,
		DF_GSE_TESTS_SCRIPT = 1 << 15,
		DF_GSE_PROMPT_JS = 1 << 16,
		DF_NOPINGS = 1 << 17,
	};
#endif

	const std::string GetEnv( const std::string& var ) const;

	const std::string& GetPrefix() const;
	const std::string& GetDataPath() const;
	const std::vector< std::string > GetPossibleSMACPaths() const;

#ifdef DEBUG

	const std::string GetDebugPath() const; // to store debug stuff like dumps
#endif

	const bool HasLaunchFlag( const launch_flag_t flag ) const;
	const types::Vec2< size_t >& GetWindowSize() const;

#ifdef DEBUG

	const bool HasDebugFlag( const debug_flag_t flag ) const;
	const util::random::state_t& GetQuickstartSeed() const;
	const std::string& GetQuickstartMapDump() const;
	const std::string& GetQuickstartMapFile() const;
	const types::Vec2< size_t >& GetQuickstartMapSize() const;
	const game::settings::map_config_value_t GetQuickstartMapOcean() const;
	const game::settings::map_config_value_t GetQuickstartMapErosive() const;
	const game::settings::map_config_value_t GetQuickstartMapLifeforms() const;
	const game::settings::map_config_value_t GetQuickstartMapClouds() const;
	const std::string& GetGSETestsScript() const;

#endif

private:

	util::ArgParser* m_parser = nullptr;

	void Error( const std::string& error );
	const types::Vec2< size_t > ParseSize( const std::string& value );
	void CheckAndSetSMACPath( const std::string& path );

	const std::string DEFAULT_GLSMAC_PREFIX =
#ifdef _WIN32
		GetEnv( "APPDATA" ) + "/GLSMAC"
#else
		GetEnv( "HOME" ) + "/.local/share/glsmac"
#endif
	;

	std::string m_prefix;
	std::string m_data_path;
	std::string m_smac_path;

	uint8_t m_launch_flags = LF_NONE;
	types::Vec2< size_t > m_window_size = {};

#ifdef DEBUG

	uint32_t m_debug_flags = DF_NONE;
	util::random::state_t m_quickstart_seed = {};
	std::string m_quickstart_mapdump = "";
	std::string m_quickstart_mapfile = "";
	types::Vec2< size_t > m_quickstart_mapsize = {};
	game::settings::map_config_value_t m_quickstart_map_ocean = game::settings::MAP_CONFIG_OCEAN_MEDIUM;
	game::settings::map_config_value_t m_quickstart_map_erosive = game::settings::MAP_CONFIG_EROSIVE_AVERAGE;
	game::settings::map_config_value_t m_quickstart_map_lifeforms = game::settings::MAP_CONFIG_LIFEFORMS_AVERAGE;
	game::settings::map_config_value_t m_quickstart_map_clouds = game::settings::MAP_CONFIG_CLOUDS_AVERAGE;

	std::string m_gse_tests_script = "";

#endif
};

}
