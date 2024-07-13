#pragma once

#include <set>
#include <string>

#include "types/Serializable.h"

#include "game/backend/rules/default/Default.h" // only default rules for now
#include "game/backend/Account.h"

#include "Types.h"

namespace game {
namespace backend {
namespace settings {

// includes

CLASS( MapSettings, types::Serializable )

	enum type_t {
		MT_RANDOM,
		MT_CUSTOM,
		MT_MAPFILE,
	};
	type_t type = MT_RANDOM;

	std::string filename = "";

	static constexpr map_config_value_t MAP_CONFIG_CUSTOM = 0;
	static constexpr map_config_value_t MAP_CONFIG_TINY = 1;
	static constexpr map_config_value_t MAP_CONFIG_SMALL = 2;
	static constexpr map_config_value_t MAP_CONFIG_STANDARD = 3;
	static constexpr map_config_value_t MAP_CONFIG_LARGE = 4;
	static constexpr map_config_value_t MAP_CONFIG_HUGE = 5;

	map_config_value_t size = MAP_CONFIG_STANDARD;
	types::Vec2< size_t > custom_size = {
		80,
		40
	};

	static constexpr map_config_value_t MAP_CONFIG_OCEAN_LOW = 1;
	static constexpr map_config_value_t MAP_CONFIG_OCEAN_MEDIUM = 2;
	static constexpr map_config_value_t MAP_CONFIG_OCEAN_HIGH = 3;
	map_config_value_t ocean = MAP_CONFIG_OCEAN_MEDIUM;

	static constexpr map_config_value_t MAP_CONFIG_EROSIVE_STRONG = 1;
	static constexpr map_config_value_t MAP_CONFIG_EROSIVE_AVERAGE = 2;
	static constexpr map_config_value_t MAP_CONFIG_EROSIVE_WEAK = 3;
	map_config_value_t erosive = MAP_CONFIG_EROSIVE_AVERAGE;

	static constexpr map_config_value_t MAP_CONFIG_LIFEFORMS_RARE = 1;
	static constexpr map_config_value_t MAP_CONFIG_LIFEFORMS_AVERAGE = 2;
	static constexpr map_config_value_t MAP_CONFIG_LIFEFORMS_ABUNDANT = 3;
	map_config_value_t lifeforms = MAP_CONFIG_LIFEFORMS_AVERAGE;

	static constexpr map_config_value_t MAP_CONFIG_CLOUDS_SPARSE = 1;
	static constexpr map_config_value_t MAP_CONFIG_CLOUDS_AVERAGE = 2;
	static constexpr map_config_value_t MAP_CONFIG_CLOUDS_DENSE = 3;
	map_config_value_t clouds = MAP_CONFIG_CLOUDS_AVERAGE;

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

// settings that are synced between players (host has authority)
CLASS( GlobalSettings, types::Serializable )

	void Initialize();

	MapSettings map = {};

	rules::Default game_rules = {}; // TODO: custom rules
	rules::DifficultyLevel global_difficulty = {};

	std::string game_name = "";

	// TODO: custom rules struct

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

// settings that aren't synced between players
CLASS( LocalSettings, types::Serializable )
public:

	Account account;

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
	std::string remote_address = "127.0.0.1";

	std::set< std::string > banned_addresses = {};

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

CLASS( Settings, types::Serializable )

	GlobalSettings global;
	LocalSettings local;

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

}
}
}
