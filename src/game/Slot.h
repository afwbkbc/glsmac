#pragma once

#include "types/Serializable.h"

#include "game/Player.h"

namespace game {

CLASS( Slot, types::Serializable )

	enum slot_state_t {
		SS_CLOSED,
		SS_OPEN,
		SS_PLAYER,
	};
	const slot_state_t GetState() const;

	void Open();
	void Close();
	void SetCloseAfterClear();

	size_t GetCid() const;
	const std::string& GetRemoteAddress() const;
	const bool IsReady() const;

	Player* GetPlayer() const;
	Player* GetPlayerAndClose();
	void SetPlayer( Player* player, const size_t cid, const std::string& remote_address );
	void SetReady( const bool ready );

	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );

private:
	
	slot_state_t m_slot_state = SS_OPEN;
	bool m_close_after_clear = false;
	struct {
		Player* player = nullptr;
		size_t cid = 0;
		std::string remote_address = "";
		bool ready = false;
	} m_player_data = {};
	
};

}
