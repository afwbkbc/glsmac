#pragma once

#include <cstdint>
#include <unordered_set>
#include <unordered_map>

#include "base/Base.h"

#include "Settings.h"
#include "Player.h"
#include "Slots.h"

#include "connection/Connection.h"

#include "network/types.h"

namespace game {

CLASS( State, base::Base )

	State();
	virtual ~State();

	Settings m_settings = {};
	Slots m_slots = {};

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
	void Reset();
	void DetachConnection();

private:
	std::unordered_set< Player* > m_players = {}; // persistent
	std::unordered_map< network::cid_t, size_t > m_cid_slots = {}; // volatile ( { cid, slot_num } )

	connection::Connection* m_connection = nullptr;
};

}
