#pragma once

namespace game {

class Settings {
public:
	
	typedef uint8_t map_parameter_t;
	
	enum game_mode_t {
		GM_SINGLEPLAYER,
		GM_MULTIPLAYER,
		GM_SCENARIO,
	};
	game_mode_t game_mode = GM_SINGLEPLAYER;
	
	enum map_type_t {
		MT_RANDOM,
		MT_CUSTOM,
		MT_PREGEN,
	};
	map_type_t map_type = MT_RANDOM;
	
	static const map_parameter_t MAP_CUSTOM = 0;
	static const map_parameter_t MAP_TINY = 1;
	static const map_parameter_t MAP_SMALL = 2;
	static const map_parameter_t MAP_STANDARD = 3;
	static const map_parameter_t MAP_LARGE = 4;
	static const map_parameter_t MAP_HUGE = 5;
	
	map_parameter_t map_size = MAP_STANDARD;
	struct {
		size_t width;
		size_t height;
	} map_custom_size = { 20, 40 };
	
	static const map_parameter_t MAP_OCEAN_SMALL = 1;
	static const map_parameter_t MAP_OCEAN_AVERAGE = 2;
	static const map_parameter_t MAP_OCEAN_LARGE = 3;
	map_parameter_t map_ocean = MAP_OCEAN_AVERAGE;
	
	static const map_parameter_t MAP_EROSIVE_STRONG = 1;
	static const map_parameter_t MAP_EROSIVE_AVERAGE = 2;
	static const map_parameter_t MAP_EROSIVE_WEAK = 3;
	map_parameter_t map_erosive = MAP_EROSIVE_AVERAGE;
	
	static const map_parameter_t MAP_LIFEFORMS_RARE = 1;
	static const map_parameter_t MAP_LIFEFORMS_AVERAGE = 2;
	static const map_parameter_t MAP_LIFEFORMS_ABUNDANT = 3;
	map_parameter_t map_lifeforms = MAP_LIFEFORMS_AVERAGE;
	
	static const map_parameter_t MAP_CLOUDS_SPARSE = 1;
	static const map_parameter_t MAP_CLOUDS_AVERAGE = 2;
	static const map_parameter_t MAP_CLOUDS_DENSE = 3;
	map_parameter_t map_clouds = MAP_CLOUDS_AVERAGE;
	
	static const map_parameter_t DIFFICULTY_CITIZEN = 1;
	static const map_parameter_t DIFFICULTY_SPECIALIST = 2;
	static const map_parameter_t DIFFICULTY_TALENT= 3;
	static const map_parameter_t DIFFICULTY_LIBRARIAN = 4;
	static const map_parameter_t DIFFICULTY_THINKER = 5;
	static const map_parameter_t DIFFICULTY_TRANSCEND = 6;
	map_parameter_t difficulty = DIFFICULTY_CITIZEN;
	
	enum game_rules_t {
		GR_STANDARD,
		GR_CURRENT,
		GR_CUSTOM,
	};
	game_rules_t game_rules = GR_STANDARD;
	
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
	
	// TODO: custom rules struct
	
	
	
};

}
