#pragma once

#include "base/Module.h"
#include "engine/Engine.h"
#include "network/Network.h"
#include "ui/module/Loader.h"
#include "game/Slot.h"
#include "game/Player.h"

namespace game {
class State;
namespace connection {

class Client;

class Server;

CLASS( Connection, base::Module )

	enum game_state_t {
		GS_NONE,
		GS_START_GAME,
		GS_INIT_MAP,
	};

	Connection( const network::connection_mode_t connection_mode, LocalSettings* const settings );
	virtual ~Connection();

	std::function< void() > m_on_connect = nullptr;
	std::function< void() > m_on_cancel = nullptr;
	std::function< void() > m_on_disconnect = nullptr;
	std::function< void( const std::string& message ) > m_on_error = nullptr;

	std::function< void() > m_on_global_settings_update = nullptr;
	std::function< void( const size_t slot_num, Slot* slot, const Player* player ) > m_on_player_join = nullptr;
	std::function< void( const size_t slot_num, Slot* slot, const Player* player ) > m_on_player_leave = nullptr;
	std::function< void( const size_t slot_num, game::Slot* slot ) > m_on_slot_update = nullptr;
	std::function< void( const std::string& message ) > m_on_message = nullptr;

	void SetState( State* state );

	virtual void ResetHandlers();

	void Connect();
	void Disconnect( const std::string& message = "" );

	void Iterate() override;

	Client* AsClient() const; // for client-specific calls
	void IfClient( std::function< void( Client* client ) > cb ); // call cb if client
	Server* AsServer() const; // for server-specific calls
	void IfServer( std::function< void( Server* server ) > cb ); // call cb if server

	const bool IsConnected() const;
	const bool IsServer() const;
	const bool IsClient() const;
	const size_t GetSlotNum() const;
	const Player* GetPlayer() const;

	virtual void UpdateSlot( const size_t slot_num, const Slot* slot ) = 0;
	virtual void Message( const std::string& message ) = 0;

protected:
	network::Network* const m_network = g_engine->GetNetwork();

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
#include "Client.h"
#include "Server.h"