#pragma once

#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "gse/GCWrappable.h"

#include "network/Types.h"

#include "gse/Exception.h"
#include "game/backend/settings/Settings.h"

class GLSMAC;

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

CLASS( State, gse::GCWrappable )
	
	State( gc::Space* const gc_space, gse::context::Context* const ctx, GLSMAC* glsmac );
	virtual ~State();
	
	void SetGame( Game* game );
	void UnsetGame();
	Game* GetGame() const;
	
	typedef std::function< void( GSE_CALLABLE ) > gse_func_t;
	
	void WithGSE( gc::Object* const owner, const gse_func_t& f, const f_cleanup_t& f_cleanup = nullptr );
	
	settings::Settings m_settings = {};
	slot::Slots* m_slots;
	
	Bindings* m_bindings = nullptr;
	
	std::function< void( const gse::Exception& ) > m_on_gse_error = nullptr;
	
	void Iterate();
	
	bool IsMaster();
	bool IsSlave();
	
	void AddPlayer( Player* player );
	void RemovePlayer( Player* player );
	
	// used only on host
	void AddCIDSlot( const network::cid_t cid, const size_t slot );
	void RemoveCIDSlot( const network::cid_t cid );
	
	const std::unordered_map< network::cid_t, size_t >& GetCidSlots() const;
	
	void SetConnection( connection::Connection* connection );
	connection::Connection* GetConnection();
	void InitBindings();
	void Configure();
	void Reset();
	void DetachConnection();
	
	faction::FactionManager* GetFM() const;
	
	gse::Value* const TriggerObject( gse::GCWrappable* object, const std::string& event, const f_args_t& f_args );
	
	WRAPDEFS_PTR( State )
	
	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );
	
	// not const yet because in legacy ui these will be set by bindings
	gc::Space* m_gc_space = nullptr;
	gse::context::Context* m_ctx = nullptr;
	
	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

private:
	
	GLSMAC* m_glsmac = nullptr;
	
	faction::FactionManager* m_fm = nullptr;
	Game* m_game = nullptr;
	
	std::unordered_set< Player* > m_players = {}; // persistent
	std::unordered_map< network::cid_t, size_t > m_cid_slots = {}; // volatile ( { cid, slot_num } )
	
	std::mutex m_connection_mutex;
	connection::Connection* m_connection = nullptr;
	
};

}
}
