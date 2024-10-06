#include "Slot.h"

namespace game {
namespace frontend {

Slot::Slot(
	const size_t slot_index,
	faction::Faction* faction
)
	: m_slot_index( slot_index )
	, m_faction( faction ) {
	//
}

const size_t Slot::GetIndex() const {
	return m_slot_index;
}

faction::Faction* Slot::GetFaction() const {
	return m_faction;
}

}
}
