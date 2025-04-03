#pragma once

#include <string>

#include "common/Module.h"

#include "util/random/Types.h"
#include "game/backend/settings/Types.h"
#include "types/Vec2.h"

#include "Types.h"

namespace util {
class ConfigManager;
}

namespace config {

CLASS( Config, common::Module )
	Config( const std::string& path );
	~Config();

	void Init( const int argc, const char* argv[] );

	enum launch_flag_t : uint32_t {
		LF_NONE = 0,
		LF_BENCHMARK = 1 << 0,
		LF_SHOWFPS = 1 << 1,
		LF_NOSOUND = 1 << 2,
		LF_SKIPINTRO = 1 << 3,
		LF_WINDOWED = 1 << 4,
		LF_WINDOW_SIZE = 1 << 5,
		LF_QUICKSTART = 1 << 6,
		LF_QUICKSTART_SEED = 1 << 7,
		LF_QUICKSTART_MAP_FILE = 1 << 8,
		LF_QUICKSTART_MAP_SIZE = 1 << 9,
		LF_QUICKSTART_MAP_OCEAN = 1 << 10,
		LF_QUICKSTART_MAP_EROSIVE = 1 << 11,
		LF_QUICKSTART_MAP_LIFEFORMS = 1 << 12,
		LF_QUICKSTART_MAP_CLOUDS = 1 << 13,
		LF_QUICKSTART_FACTION = 1 << 14,
		LF_MODS = 1 << 15,
		LF_NEWUI = 1 << 16,
	};

#if defined( DEBUG ) || defined( FASTDEBUG )
	enum debug_flag_t : uint16_t {
		DF_NONE = 0,
		DF_GDB = 1 << 0,
		DF_QUIET = 1 << 1,
		DF_GSE_ONLY = 1 << 2,
		DF_GSE_TESTS = 1 << 3,
		DF_GSE_TESTS_SCRIPT = 1 << 4,
		DF_GSE_PROMPT_JS = 1 << 5,
		DF_NOPINGS = 1 << 6,
		DF_VERBOSE_GC = 1 << 7,
		DF_NO_GC = 1 << 8,
		DF_SINGLE_THREAD = 1 << 9,
#ifdef DEBUG
		DF_MAPDUMP = 1 << 10,
		DF_MEMORYDEBUG = 1 << 11,
		DF_QUICKSTART_MAP_DUMP = 1 << 12,
#endif
	};
#endif

	const std::string GetEnv( const std::string& var ) const;
	const std::string& GetPrefix() const;
	const std::string& GetDataPath() const;
	const std::vector< std::string > GetPossibleSMACPaths() const;

	const smac_type_t GetSMACType() const;

	void SetSMACPath( const std::string& path ) const;

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string GetDebugPath() const; // to store debug stuff like dumps
#endif

	const bool HasLaunchFlag( const launch_flag_t flag ) const;
	const types::Vec2< size_t >& GetWindowSize() const;

	const util::random::state_t& GetQuickstartSeed() const;
	const std::string& GetQuickstartMapFile() const;
	const types::Vec2< size_t >& GetQuickstartMapSize() const;
	const float GetQuickstartMapOceanCoverage() const;
	const float GetQuickstartMapErosiveForces() const;
	const float GetQuickstartMapNativeLifeforms() const;
	const float GetQuickstartMapCloudCover() const;
	const std::string& GetQuickstartFaction() const;

	const std::vector< std::string >& GetModPaths() const;

	const std::string& GetNewUIMainScript() const;

#if defined( DEBUG ) || defined( FASTDEBUG )

	const bool HasDebugFlag( const debug_flag_t flag ) const;
	const std::string& GetQuickstartMapDump() const;
	const std::string& GetGSETestsScript() const;

#endif

private:

	util::ConfigManager* m_manager = nullptr;

	void Error( const std::string& error );
	const types::Vec2< size_t > ParseSize( const std::string& value );

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
	smac_type_t m_smac_type = ST_AUTO;

	uint32_t m_launch_flags = LF_NONE;
	types::Vec2< size_t > m_window_size = {};

	util::random::state_t m_quickstart_seed = {};
	std::string m_quickstart_mapfile = "";
	types::Vec2< size_t > m_quickstart_mapsize = {};
	float m_quickstart_map_ocean_coverage = 0.4f;
	float m_quickstart_map_erosive_forces = 0.75f;
	float m_quickstart_map_native_lifeforms = 0.5f;
	float m_quickstart_map_cloud_cover = 0.5f;
	std::string m_quickstart_faction = "";

	std::vector< std::string > m_mod_paths = {};

	std::string m_newui_mainscript = "main";

#if defined( DEBUG ) || defined( FASTDEBUG )

	uint16_t m_debug_flags = DF_NONE;
	std::string m_quickstart_mapdump = "";

	std::string m_gse_tests_script = "";

#endif
};

}
