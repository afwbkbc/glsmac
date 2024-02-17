#pragma once

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "base/Base.h"

#include "Settings.h"
#include "Player.h"
#include "Slots.h"

#include "connection/Connection.h"
#include "network/types.h"

#include "bindings/Bindings.h"

namespace game {

class Game;

CLASS( State, base::Base )

	State();
	virtual ~State();

	void SetGame( Game* game );
	void UnsetGame();
	Game* GetGame() const;

	Settings m_settings = {};
	Slots m_slots = { this };

	bindings::Bindings* m_bindings = nullptr;

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

private:
	Game* m_game = nullptr;

	std::unordered_set< Player* > m_players = {}; // persistent
	std::unordered_map< network::cid_t, size_t > m_cid_slots = {}; // volatile ( { cid, slot_num } )

	connection::Connection* m_connection = nullptr;

};

}
