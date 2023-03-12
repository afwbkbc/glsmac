#pragma once

#include <string>

#include "base/Module.h"

#include "util/Random.h"
#include "types/Vec2.h"

namespace config {

CLASS( Config, base::Module )
	Config( const int argc, const char *argv[] );
	
	enum launch_flag_t : uint8_t {
		LF_NONE = 0,
		LF_BENCHMARK = 1 << 0
	};
	
#ifdef DEBUG
	enum debug_flag_t : uint8_t {
		DF_NONE = 0,
		DF_GDB = 1 << 0,
		DF_MEMORYDEBUG = 1 << 1,
		DF_QUICKSTART = 1 << 2,
		DF_QUICKSTART_SEED = 1 << 3,
		DF_QUICKSTART_MAPDUMP = 1 << 4,
		DF_QUICKSTART_MAPFILE = 1 << 5,
		DF_QUICKSTART_MAPSIZE = 1 << 6
	};
#endif
	
	const std::string& GetSMACPath() const;
	
	const bool HasLaunchFlag( const launch_flag_t flag ) const;
#ifdef DEBUG
	const bool HasDebugFlag( const debug_flag_t flag ) const;
	const util::Random::state_t& GetQuickstartSeed() const;
	const std::string& GetQuickstartMapDump() const;
	const std::string& GetQuickstartMapFile() const;
	const types::Vec2< size_t >& GetQuickstartMapSize() const;
#endif
	
private:
	const int m_argc = 0;
	const char** const m_argv = nullptr;
	
	std::string m_smac_path = "";
	
	uint8_t m_launch_flags = LF_NONE;
#ifdef DEBUG
	uint8_t m_debug_flags = DF_NONE;
	util::Random::state_t m_quickstart_seed = {};
	std::string m_quickstart_mapdump = "";
	std::string m_quickstart_mapfile = "";
	types::Vec2< size_t > m_quickstart_mapsize = {};
#endif
};

} /* namespace config */
