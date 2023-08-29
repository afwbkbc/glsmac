#pragma once

#include <set>
#include <string>

#include "types/Serializable.h"

// only default rules for now
#include "rules/default/Default.h"

using namespace types;

namespace game {

// includes
	
CLASS( MapSettings, Serializable )
	typedef uint8_t parameter_t;
	
	enum type_t {
		MT_RANDOM,
		MT_CUSTOM,
		MT_MAPFILE,
	};
	type_t type = MT_RANDOM;
	
	static constexpr parameter_t MAP_CUSTOM = 0;
	static constexpr parameter_t MAP_TINY = 1;
	static constexpr parameter_t MAP_SMALL = 2;
	static constexpr parameter_t MAP_STANDARD = 3;
	static constexpr parameter_t MAP_LARGE = 4;
	static constexpr parameter_t MAP_HUGE = 5;
	
	parameter_t size = MAP_STANDARD;
	Vec2< size_t > custom_size = { 80, 40 };
	
	static constexpr parameter_t MAP_OCEAN_LOW = 1;
	static constexpr parameter_t MAP_OCEAN_MEDIUM = 2;
	static constexpr parameter_t MAP_OCEAN_HIGH = 3;
	parameter_t ocean = MAP_OCEAN_MEDIUM;
	
	static constexpr parameter_t MAP_EROSIVE_STRONG = 1;
	static constexpr parameter_t MAP_EROSIVE_AVERAGE = 2;
	static constexpr parameter_t MAP_EROSIVE_WEAK = 3;
	parameter_t erosive = MAP_EROSIVE_AVERAGE;
	
	static constexpr parameter_t MAP_LIFEFORMS_RARE = 1;
	static constexpr parameter_t MAP_LIFEFORMS_AVERAGE = 2;
	static constexpr parameter_t MAP_LIFEFORMS_ABUNDANT = 3;
	parameter_t lifeforms = MAP_LIFEFORMS_AVERAGE;
	
	static constexpr parameter_t MAP_CLOUDS_SPARSE = 1;
	static constexpr parameter_t MAP_CLOUDS_AVERAGE = 2;
	static constexpr parameter_t MAP_CLOUDS_DENSE = 3;
	parameter_t clouds = MAP_CLOUDS_AVERAGE;
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
};

// settings that are synced between players (host has authority)
CLASS( GlobalSettings, Serializable )
	typedef uint8_t parameter_t;	

	MapSettings map = {};

	// TODO: use difficulty levels from rules
	static constexpr parameter_t DIFFICULTY_CITIZEN = 1;
	static constexpr parameter_t DIFFICULTY_SPECIALIST = 2;
	static constexpr parameter_t DIFFICULTY_TALENT= 3;
	static constexpr parameter_t DIFFICULTY_LIBRARIAN = 4;
	static constexpr parameter_t DIFFICULTY_THINKER = 5;
	static constexpr parameter_t DIFFICULTY_TRANSCEND = 6;
	parameter_t difficulty = DIFFICULTY_CITIZEN;
	
	rules::Default game_rules; // TODO: custom rules


	
	std::string game_name = "";
	
	// TODO: custom rules struct
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
};

// settings that aren't synced between players
CLASS( LocalSettings, Serializable )
public:

	enum game_mode_t {
		GM_SINGLEPLAYER,
		GM_MULTIPLAYER,
		GM_SCENARIO,
	};
	game_mode_t game_mode = GM_SINGLEPLAYER;
	enum network_type_t {
		NT_NONE,
		NT_SIMPLETCP,
	};
	network_type_t network_type = NT_NONE;

	enum network_role_t {
		NR_NONE,
		NR_SERVER,
		NR_CLIENT,
	};
	network_role_t network_role = NR_NONE;

	std::string player_name = "";
	std::string remote_address = "";
	
	std::string map_file = "";
	
	std::set< std::string > banned_addresses = {};
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
};

CLASS( Settings, Serializable )
	
	GlobalSettings global;
	LocalSettings local;
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
};

}
