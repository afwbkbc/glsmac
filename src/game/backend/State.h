#pragma once

#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "common/Common.h"

#include "network/Types.h"

#include "gse/Exception.h"
#include "game/backend/settings/Settings.h"

namespace game {
namespace backend {

namespace faction {
class FactionManager;
}

class Bindings;
class Game;
class Player;

namespace connection {
class Connection;
}
namespace slot {
class Slots;
}

CLASS2( State, common::Class, gse::Wrappable )

	State();
	virtual ~State();

	void SetGame( Game* game );
	void UnsetGame();
	Game* GetGame() const;

	settings::Settings m_settings = {};
	slot::Slots* m_slots;

	Bindings* m_bindings = nullptr;

	std::function< void( gse::Exception& ) > m_on_gse_error = nullptr;

	void Iterate();

	bool IsMaster() const;
	bool IsSlave() const;

	void AddPlayer( Player* player );
	void RemovePlayer( Player* player );

	// used only on host
	void AddCIDSlot( const network::cid_t cid, const size_t slot );
	void RemoveCIDSlot( const network::cid_t cid );

	const std::unordered_map< network::cid_t, size_t >& GetCidSlots() const;

	void SetConnection( connection::Connection* connection );
	connection::Connection* GetConnection() const;
	void InitBindings();
	void Configure();
	void Reset();
	void DetachConnection();

	faction::FactionManager* GetFM() const;

	WRAPDEFS_PTR( State )

	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );

private:
	faction::FactionManager* m_fm = nullptr;
	Game* m_game = nullptr;

	std::unordered_set< Player* > m_players = {}; // persistent
	std::unordered_map< network::cid_t, size_t > m_cid_slots = {}; // volatile ( { cid, slot_num } )

	connection::Connection* m_connection = nullptr;

};

}
}
