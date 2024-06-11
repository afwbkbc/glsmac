#include "Base.h"

#include "gse/context/Context.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/callable/Native.h"
#include "game/Game.h"
#include "game/State.h"
#include "game/slot/Slot.h"
#include "game/slot/Slots.h"
#include "game/map/Map.h"

namespace game {
namespace base {

static size_t next_id = 1;
const size_t Base::GetNextId() {
	return next_id;
}
const void Base::SetNextId( const size_t id ) {
	next_id = id;
}

Base::Base(
	Game* game,
	const size_t id,
	slot::Slot* owner,
	map::tile::Tile* tile
)
	: MapObject( game->GetMap(), tile )
	, m_game( game )
	, m_id( id )
	, m_owner( owner ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
	ASSERT_NOLOG( !tile->base, "tile already has base" );
	tile->base = this;
	m_tile = tile;
	m_game->RefreshBase( this );
}

const types::Buffer Base::Serialize( const Base* unit ) {
	types::Buffer buf;
	buf.WriteInt( unit->m_id );
	buf.WriteInt( unit->m_owner->GetIndex() );
	buf.WriteInt( unit->m_tile->coord.x );
	buf.WriteInt( unit->m_tile->coord.y );
	return buf;
}

Base* Base::Unserialize( types::Buffer& buf, Game* game ) {
	const auto id = buf.ReadInt();
	auto* slot = game ? &game->GetState()->m_slots->GetSlot( buf.ReadInt() ) : nullptr;
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = game ? game->GetMap()->GetTile( pos_x, pos_y ) : nullptr;
	return new Base( game, id, slot, tile );
}

WRAPIMPL_DYNAMIC_GETTERS( Base, CLASS_BASE )
	WRAPIMPL_GET( "id", Int, m_id )
	WRAPIMPL_LINK( "get_owner", m_owner )
	WRAPIMPL_LINK( "get_tile", m_tile )
WRAPIMPL_DYNAMIC_SETTERS( Base )
WRAPIMPL_DYNAMIC_ON_SET( Base )
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Base )

}
}
