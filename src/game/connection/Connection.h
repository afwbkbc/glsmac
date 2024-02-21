#pragma once

#include <vector>

#include "base/Module.h"
#include "engine/Engine.h"
#include "network/Network.h"
#include "ui/module/Loader.h"
#include "game/Slot.h"
#include "game/Player.h"
#include "game/event/Event.h"

namespace game {
class State;
namespace connection {

class Client;

class Server;

CLASS( Connection, base::Module )

	enum game_state_t : uint8_t {
		GS_NONE,
		GS_LOBBY,
		GS_INITIALIZING,
		GS_RUNNING,
	};

	Connection( const network::connection_mode_t connection_mode, LocalSettings* const settings );
	virtual ~Connection();

	std::function< void() > m_on_connect = nullptr;

	// return true if connection should be destroyed afterwards
	std::function< bool() > m_on_cancel = nullptr;
	std::function< bool() > m_on_disconnect = nullptr;
	std::function< bool( const std::string& message ) > m_on_error = nullptr;

	std::function< void() > m_on_global_settings_update = nullptr;
	std::function< void( const size_t slot_num, Slot* slot, const Player* player ) > m_on_player_join = nullptr;
	std::function< void( const size_t slot_num, Slot* slot, const Player* player ) > m_on_player_leave = nullptr;
	std::function< void( const size_t slot_num, game::Slot* slot ) > m_on_slot_update = nullptr;
	std::function< void( const size_t slot_num, game::Slot* slot, const Slot::player_flag_t old_flags, const Slot::player_flag_t new_flags ) > m_on_flags_update = nullptr;
	std::function< void( const std::string& message ) > m_on_message = nullptr;
	std::function< void( const game::event::Event* event ) > m_on_game_event = nullptr;

	void SetState( State* state );

	virtual void ResetHandlers();

	void Connect();
	void Disconnect( const std::string& message = "" );

	void Iterate() override;

	Client* AsClient() const; // for client-specific calls
	void IfClient( std::function< void( Client* client ) > cb ); // call cb if client
	Server* AsServer() const; // for server-specific calls
	void IfServer( std::function< void( Server* server ) > cb ); // call cb if server

	void SendGameEvent( const game::event::Event* event );

	const bool IsConnected() const;
	const bool IsServer() const;
	const bool IsClient() const;
	const size_t GetSlotNum() const;
	const Player* GetPlayer() const;

	virtual void UpdateSlot( const size_t slot_num, Slot* slot, const bool only_flags = false ) = 0;
	virtual void SendMessage( const std::string& message ) = 0;

protected:
	const int DOWNLOAD_CHUNK_SIZE = 16384;

	network::Network* const m_network = g_engine->GetNetwork();

	virtual void ProcessEvent( const network::Event& event );

	bool m_is_connected = false;
	bool m_is_canceled = false; // canceled by user

	std::string m_disconnect_reason = "";
	LocalSettings* m_settings = nullptr;
	State* m_state = nullptr;

protected:
	typedef std::vector< const game::event::Event* > game_events_t;
	game_state_t m_game_state = GS_NONE;

	size_t m_slot = 0;
	::game::Player* m_player = nullptr;

	virtual void SendGameEvents( const game_events_t& game_events ) = 0;

private:
	const network::connection_mode_t m_connection_mode = network::CM_NONE;
	struct {
		mt_id_t connect = 0;
		mt_id_t events = 0;
		mt_id_t disconnect = 0;
	} m_mt_ids = {};

	// buffer events for optimization
	const size_t PENDING_GAME_EVENTS_LIMIT = 256;
	game_events_t m_pending_game_events = {};

	void ProcessPending();
	void ClearPending();

};

}
}

#include "game/State.h"
#include "Client.h"
#include "Server.h"