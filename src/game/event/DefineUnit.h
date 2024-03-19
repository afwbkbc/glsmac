#pragma once

#include "Event.h"

#include "game/unit/Def.h"

namespace game {
namespace event {

class DefineUnit : public Event {
public:
	DefineUnit( const size_t initiator_slot, unit::Def* def );

	const std::string* Validate( const Game* game ) const override;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefineUnit* event );
	static DefineUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	unit::Def* m_def;
};

}
}
