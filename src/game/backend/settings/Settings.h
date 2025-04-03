#pragma once

#include <set>
#include <string>

#include "types/Serializable.h"

#include "game/backend/rules/default/Default.h" // only default rules for now
#include "game/backend/Account.h"

#include "gse/Wrappable.h"

#include "Types.h"

namespace game {
namespace backend {
namespace settings {

// includes

CLASS2( MapSettings, types::Serializable, gse::Wrappable )

	enum type_t {
		MT_RANDOM,
		MT_CUSTOM,
		MT_MAPFILE,
	};
	type_t type = MT_RANDOM;

	std::string filename = "";

	/*
	 	{ settings::MAP_CONFIG_TINY,     { 68,  34 } },
		{ settings::MAP_CONFIG_SMALL,    { 88,  44 } },
		{ settings::MAP_CONFIG_STANDARD, { 112, 56 } },
		{ settings::MAP_CONFIG_LARGE,    { 140, 70 } },
		{ settings::MAP_CONFIG_HUGE,     { 180, 90 } }
	 */
	size_t size_x = 112;
	size_t size_y = 56;

	/*
	 	{ settings::MAP_CONFIG_OCEAN_LOW,    0.4f }, // '30-50% of surface'
		{ settings::MAP_CONFIG_OCEAN_MEDIUM, 0.6f }, // '50-70% of surface'
		{ settings::MAP_CONFIG_OCEAN_HIGH,   0.8f }  // '70-90% of surface'
	 */
	float ocean_coverage = 0.4f;

	/*
	 	{ settings::MAP_CONFIG_EROSIVE_STRONG,  0.5f }, // 'strong'
		{ settings::MAP_CONFIG_EROSIVE_AVERAGE, 0.75f }, // 'average'
		{ settings::MAP_CONFIG_EROSIVE_WEAK,    1.0f }, // 'weak'
	 */
	float erosive_forces = 0.75f;

	/*
 		{ settings::MAP_CONFIG_LIFEFORMS_RARE,     0.25f }, // 'rare'
		{ settings::MAP_CONFIG_LIFEFORMS_AVERAGE,  0.5f }, // 'average'
		{ settings::MAP_CONFIG_LIFEFORMS_ABUNDANT, 0.75f }, // 'abundant'
	 */
	float native_lifeforms = 0.5f;

	/*
	 	{ settings::MAP_CONFIG_CLOUDS_SPARSE,  0.25f }, // 'sparse'
		{ settings::MAP_CONFIG_CLOUDS_AVERAGE, 0.5f }, // 'average'
		{ settings::MAP_CONFIG_CLOUDS_DENSE,   0.75f }, // 'dense'
	 */
	float cloud_cover = 0.5f;

	WRAPDEFS_DYNAMIC( MapSettings );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

// settings that are synced between players (host has authority)
CLASS2( GlobalSettings, types::Serializable, gse::Wrappable )

	void Initialize();

	MapSettings map = {};

	rules::Default game_rules = {}; // TODO: custom rules
	rules::DifficultyLevel global_difficulty = {};

	std::string game_name = "";

	// TODO: custom rules struct

	WRAPDEFS_DYNAMIC( GlobalSettings );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

// settings that aren't synced between players
CLASS2( LocalSettings, types::Serializable, gse::Wrappable )
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

	WRAPDEFS_DYNAMIC( LocalSettings );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

CLASS2( Settings, types::Serializable, gse::Wrappable )

	GlobalSettings global;
	LocalSettings local;

	WRAPDEFS_PTR( Settings );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

}
}
}
