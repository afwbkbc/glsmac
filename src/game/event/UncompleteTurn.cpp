#include "UncompleteTurn.h"

#include "../Game.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

UncompleteTurn::UncompleteTurn( const size_t initiator_slot )
	: Event( initiator_slot, ET_UNCOMPLETE_TURN ) {
	//
}

const std::string* UncompleteTurn::Validate( const Game* game ) const {
	if ( game->IsTurnActive() ) {
		return new std::string( "Turn not completed" );
	}
	return nullptr;
}

const gse::Value UncompleteTurn::Apply( game::Game* game ) const {
	game->UncompleteTurn();
	return VALUE( gse::type::Undefined );
}

TS_BEGIN( UncompleteTurn )
		TS_FUNC_BEGIN( "UncompleteTurn" ) +
		TS_FUNC_END()
TS_END()

void UncompleteTurn::Serialize( types::Buffer& buf, const UncompleteTurn* event ) {
}

UncompleteTurn* UncompleteTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	return new UncompleteTurn( initiator_slot );
}

}
}
