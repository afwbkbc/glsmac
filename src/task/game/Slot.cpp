#include "Slot.h"

namespace task {
namespace game {

Slot::Slot(
	const size_t slot_index,
	const faction::Faction* faction
)
	: m_slot_index( slot_index )
	, m_faction( faction ) {
	//
}

const size_t Slot::GetIndex() const {
	return m_slot_index;
}

const faction::Faction* Slot::GetFaction() const {
	return m_faction;
}

}
}
