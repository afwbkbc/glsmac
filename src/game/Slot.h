#pragma once

#include "types/Serializable.h"

#include "game/Player.h"
#include "network/Network.h"

#include "gse/Value.h"

namespace game {

class State;

CLASS( Slot, types::Serializable )

	Slot( const size_t index, const State* state );

	enum slot_state_t {
		SS_CLOSED,
		SS_OPEN,
		SS_PLAYER,
	};
	const slot_state_t GetState() const;

	typedef uint8_t player_flag_t;
	static const player_flag_t PF_NONE = 0;
	static const player_flag_t PF_READY = 1 << 0;
	static const player_flag_t PF_MAP_DOWNLOADED = 1 << 1;
	static const player_flag_t PF_GAME_INITIALIZED = 1 << 2;

	void Open();
	void Close();
	void SetCloseAfterClear();

	const size_t GetIndex() const;
	const network::cid_t GetCid() const;
	const std::string& GetRemoteAddress() const;
	void SetPlayerFlag( const player_flag_t flag );
	const bool HasPlayerFlag( const player_flag_t flag ) const;
	void UnsetPlayerFlag( const player_flag_t flag );

	Player* GetPlayer() const;
	Player* GetPlayerAndClose();
	void SetPlayer( Player* player, const network::cid_t cid, const std::string& remote_address );

	const std::string& GetLinkedGSID() const;
	void SetLinkedGSID( const std::string& gsid );
	void UnsetLinkedGSID();

	const player_flag_t GetPlayerFlags() const;
	void SetPlayerFlags( const player_flag_t flags );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

	WRAPDEFS_PTR( Slot );

private:

	size_t m_index;
	const State* m_state;

	slot_state_t m_slot_state = SS_OPEN;
	bool m_close_after_clear = false;
	struct {
		Player* player = nullptr;
		network::cid_t cid = 0;
		std::string remote_address = "";
		player_flag_t flags = PF_NONE;
	} m_player_data = {};
	std::string m_linked_gsid = "";
};

}
