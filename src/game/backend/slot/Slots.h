#include <vector>

#include "types/Serializable.h"

#include "Slot.h"

namespace game {
namespace backend {

class State;

namespace slot {

CLASS( Slots, types::Serializable )

	Slots( State* state );

	const size_t GetCount() const;
	void Resize( const size_t size );
	Slot& GetSlot( const size_t index );
	std::vector< Slot >& GetSlots();
	void Clear();

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;
	void DeserializeUpdate( types::Buffer buf );

private:
	State* m_state;

	std::vector< Slot > m_slots = {};
	static constexpr size_t MAX_SERIALIZED_SLOTS = 64;

};

}
}
}
