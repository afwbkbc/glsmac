#pragma once

#include "types/Serializable.h"
#include "gse/Wrappable.h"

#include "Types.h"
#include "network/Types.h"

namespace game {
namespace backend {

class State;
class Player;

namespace slot {

CLASS2( Slot, types::Serializable, gse::Wrappable )

	Slot( const size_t index, State* state );

	enum slot_state_t {
		SS_CLOSED,
		SS_OPEN,
		SS_PLAYER,
	};
	const slot_state_t GetState() const;

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
	void Deserialize( types::Buffer buf ) override;

	WRAPDEFS_PTR( Slot );

private:

	size_t m_index;
	State* m_state;

	slot_state_t m_slot_state = SS_OPEN;
	bool m_close_after_clear = false;
	struct {
		Player* player = nullptr;
		network::cid_t cid = 0;
		std::string remote_address = "";
		player_flag_t flags = PF_NONE;
	} m_player_data = {};
	std::string m_linked_gsid = "";

	void Update();
};

}
}
}
