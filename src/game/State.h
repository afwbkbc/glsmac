#pragma once

#include "base/Base.h"

#include "Settings.h"
#include "Player.h"
#include "Slots.h"

namespace game {

CLASS( State, base::Base )
	
	State( const Settings& settings );
	~State();

	Settings m_settings = {};
	Slots m_slots = {};

	void AddPlayer( Player* player );
	void RemovePlayer( Player* player );

	// used only on host
	void AddCIDSlot( const size_t cid, const size_t slot );
	void RemoveCIDSlot( const size_t cid );

	const std::unordered_map< size_t, size_t >& GetCidSlots() const;

private:
	std::unordered_set< Player* > m_players = {}; // persistent
	std::unordered_map< size_t, size_t > m_cid_slots = {}; // volatile ( { cid, slot } )

};

}
