#pragma once

#include "base/Module.h"
#include "engine/Engine.h"
#include "network/Network.h"
#include "ui/module/Loader.h"
#include "game/Slot.h"
#include "game/Player.h"

typedef std::function<bool()> connection_handler_t;
#define CH( ... ) [ __VA_ARGS__ ] () -> void

namespace game {
class State;
namespace connection {

CLASS( Connection, base::Module )

	Connection( const network::connection_mode_t connection_mode, LocalSettings* const settings );
	virtual ~Connection();

	// protocol
	std::function<void()> m_on_connect = 0;
	std::function<void()> m_on_cancel = 0;
	std::function<void()> m_on_disconnect = 0;
	std::function<void(const std::string &message)> m_on_error = 0;

	// universal
	std::function<void( const size_t slot_num, Slot* slot, const Player* player )> m_on_player_join = 0;
	std::function<void( const size_t slot_num, Slot* slot, const Player* player )> m_on_player_leave = 0;
	std::function<void( const size_t slot_num, game::Slot* slot )> m_on_slot_update = 0;

	// client-specific
	std::function<void()> m_on_global_settings_update = 0;
	std::function<void()> m_on_players_list_update = 0;

	// server-specific
	std::function<void()> m_on_listen = 0;
	
	void SetState( State* state );

	void Connect();
	void Disconnect( const std::string& message = "" );

	void Iterate();
	
	const Player* GetPlayer() const;

	virtual void UpdateSlot( const size_t slot_num, const Slot* slot ) = 0;
	
protected:
	network::Network * const m_network = g_engine->GetNetwork();

	virtual void ProcessEvent( const network::Event& event );
	
	bool m_is_connected = false;

	std::string m_disconnect_reason = "";
	LocalSettings* m_settings = nullptr;
	State* m_state = nullptr;

protected:
	size_t m_slot = 0;
	::game::Player* m_player = nullptr;

private:
	const network::connection_mode_t m_connection_mode = network::CM_NONE;
	struct {
		mt_id_t connect = 0;
		mt_id_t events = 0;
		mt_id_t disconnect = 0;
	} m_mt_ids = {};

};

}
}

#include "game/State.h"
