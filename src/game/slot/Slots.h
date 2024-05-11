#include <vector>

#include "types/Serializable.h"

#include "game/slot/Slot.h"

namespace game {

class State;

namespace slot {

CLASS( Slots, types::Serializable )

	Slots( const State* state );

	const size_t GetCount() const;
	void Resize( const size_t size );
	Slot& GetSlot( const size_t index );
	std::vector< Slot >& GetSlots();
	void Clear();

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

private:
	const State* m_state;

	std::vector< Slot > m_slots = {};

};

}
}
