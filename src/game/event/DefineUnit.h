#pragma once

#include "Event.h"

#include "game/unit/Def.h"

namespace game {
namespace event {

class DefineUnit : public Event {
public:
	DefineUnit( const unit::Def* def );

	const gse::Value Apply( game::Game* game ) const override;

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefineUnit* event );
	static DefineUnit* Unserialize( types::Buffer& buf );

private:
	const unit::Def* m_def;
};

}
}
