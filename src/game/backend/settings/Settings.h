#pragma once

#include <set>
#include <string>
#include <atomic>

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
	std::atomic< type_t > type = MT_RANDOM;

	std::string filename = "";

	/*
	 	{ settings::MAP_CONFIG_TINY,     { 68,  34 } },
		{ settings::MAP_CONFIG_SMALL,    { 88,  44 } },
		{ settings::MAP_CONFIG_STANDARD, { 112, 56 } },
		{ settings::MAP_CONFIG_LARGE,    { 140, 70 } },
		{ settings::MAP_CONFIG_HUGE,     { 180, 90 } }
	 */
	std::atomic< int64_t > size_x = 112;
	std::atomic< int64_t > size_y = 56;

	/*
	 	{ settings::MAP_CONFIG_OCEAN_LOW,    0.4f }, // '30-50% of surface'
		{ settings::MAP_CONFIG_OCEAN_MEDIUM, 0.6f }, // '50-70% of surface'
		{ settings::MAP_CONFIG_OCEAN_HIGH,   0.8f }  // '70-90% of surface'
	 */
	std::atomic< float > ocean_coverage = 0.4f;

	/*
	 	{ settings::MAP_CONFIG_EROSIVE_STRONG,  0.5f }, // 'strong'
		{ settings::MAP_CONFIG_EROSIVE_AVERAGE, 0.75f }, // 'average'
		{ settings::MAP_CONFIG_EROSIVE_WEAK,    1.0f }, // 'weak'
	 */
	std::atomic< float > erosive_forces = 0.75f;

	/*
 		{ settings::MAP_CONFIG_LIFEFORMS_RARE,     0.25f }, // 'rare'
		{ settings::MAP_CONFIG_LIFEFORMS_AVERAGE,  0.5f }, // 'average'
		{ settings::MAP_CONFIG_LIFEFORMS_ABUNDANT, 0.75f }, // 'abundant'
	 */
	std::atomic< float > native_lifeforms = 0.5f;

	/*
	 	{ settings::MAP_CONFIG_CLOUDS_SPARSE,  0.25f }, // 'sparse'
		{ settings::MAP_CONFIG_CLOUDS_AVERAGE, 0.5f }, // 'average'
		{ settings::MAP_CONFIG_CLOUDS_DENSE,   0.75f }, // 'dense'
	 */
	std::atomic< float > cloud_cover = 0.5f;

	WRAPDEFS_DYNAMIC( MapSettings );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;
};

// settings that are synced between players (host has authority)
CLASS2( GlobalSettings, types::Serializable, gse::Wrappable )

	void Initialize();

	MapSettings map = {};

	rules::Default rules = {}; // TODO: custom rules
	std::atomic< int64_t > difficulty_level = rules.GetDefaultDifficultyLevelV();

	std::string game_name = "";

	// TODO: custom rules struct

	WRAPDEFS_DYNAMIC( GlobalSettings );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;
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
	std::atomic< game_mode_t > game_mode = GM_SINGLEPLAYER;

	enum network_type_t {
		NT_NONE,
		NT_SIMPLE_TCPIP,
		NT_HOTSEAT,
	};
	std::atomic< network_type_t > network_type = NT_NONE;

	enum network_role_t {
		NR_NONE,
		NR_SERVER,
		NR_CLIENT,
	};
	std::atomic< network_role_t > network_role = NR_NONE;

	std::string player_name = "";
	std::string remote_address = "127.0.0.1";

	std::set< std::string > banned_addresses = {};

	WRAPDEFS_DYNAMIC( LocalSettings );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;
};

CLASS2( Settings, types::Serializable, gse::Wrappable )

	GlobalSettings global;
	LocalSettings local;

	WRAPDEFS_PTR( Settings );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;
};

}
}
}
