#pragma once

#include "TileLocksEvent.h"

namespace game {
namespace backend {
namespace event {

class RequestTileUnlocks : public TileLocksEvent {
public:
	RequestTileUnlocks(
		const size_t initiator_slot,
		const map::tile::positions_t& tile_positions,
		const size_t nothing = 0
	);

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const RequestTileUnlocks* event );
	static RequestTileUnlocks* Unserialize( types::Buffer& buf, const size_t initiator_slot );
};

}
}
}
