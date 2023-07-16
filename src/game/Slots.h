#include <vector>

#include "types/Serializable.h"
#include "Slot.h"

namespace game {

CLASS( Slots, types::Serializable )

	const size_t GetCount() const;
	void Resize( const size_t size );
	Slot& GetSlot( const size_t index );
	const std::vector< Slot >& GetSlots() const;

	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );

private:
	std::vector< Slot > m_slots = {};
	
};

}
