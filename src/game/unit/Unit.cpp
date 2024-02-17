#include "Unit.h"

#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/callable/Native.h"

#include "game/Game.h"
#include "game/State.h"

namespace game {
namespace unit {

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}
const void Unit::SetNextId( const size_t id ) {
	next_id = id;
}

Unit::Unit( const size_t id, const Def* def, Slot* owner, map::Tile* tile )
	: m_id( id )
	, m_def( def )
	, m_owner( owner )
	, m_tile( tile ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
}

const types::Buffer Unit::Serialize( const Unit* unit ) {
	types::Buffer buf;
	buf.WriteInt( unit->m_id );
	buf.WriteString( Def::Serialize( unit->m_def ).ToString() );
	buf.WriteInt( unit->m_owner->GetIndex() );
	buf.WriteInt( unit->m_tile->coord.x );
	buf.WriteInt( unit->m_tile->coord.y );
	return buf;
}

Unit* Unit::Unserialize( types::Buffer& buf, const Game* game ) {
	const auto id = buf.ReadInt();
	auto defbuf = types::Buffer( buf.ReadString() );
	const auto* def = Def::Unserialize( defbuf );
	auto* slot = game ? &game->GetState()->m_slots.GetSlot( buf.ReadInt() ) : nullptr;
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = game ? game->GetMap()->GetTile( pos_x, pos_y ) : nullptr;
	return new Unit( id, def, slot, tile );
}

WRAPIMPL_BEGIN( Unit, CLASS_UNIT )
	WRAPIMPL_PROPS {
		{
			"id",
			VALUE( gse::type::Int, m_id )
		},
		{
			"get_def",
			NATIVE_CALL( this ) {
				return m_def->Wrap();
			})
		},
		{
			"get_owner",
			NATIVE_CALL( this ) {
				return m_owner->Wrap();
			})
		},
		{
			"get_tile",
			NATIVE_CALL( this ) {
				return m_tile->Wrap();
			})
		}
	};
WRAPIMPL_END_PTR( Unit )

UNWRAPIMPL_PTR( Unit )

}
}
