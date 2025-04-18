#pragma once

#include "TileLocksEvent.h"

namespace game {
namespace backend {
namespace event {

class UnlockTiles : public TileLocksEvent {
public:
	UnlockTiles(
		const size_t initiator_slot,
		const map::tile::positions_t& tile_positions,
		const size_t lock_owner_slot
	);

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const UnlockTiles* event );
	static UnlockTiles* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_lock_owner_slot;
};

}
}
}
