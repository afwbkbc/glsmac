#include "Base.h"

#include "gse/context/Context.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Undefined.h"
#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/Faction.h"
#include "Pop.h"
#include "game/backend/bindings/Binding.h"
#include "PopDef.h"
#include "util/random/Random.h"

namespace game {
namespace backend {
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
	Faction* faction,
	map::tile::Tile* tile,
	const std::string& name,
	const pops_t& pops
)
	: MapObject( game->GetMap(), tile )
	, m_game( game )
	, m_id( id )
	, m_owner( owner )
	, m_faction( faction )
	, m_name( name )
	, m_pops( pops ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
	ASSERT_NOLOG( !tile->base, "tile already has base" );
	tile->base = this;
	m_tile = tile;
}

void Base::AddPop( const Pop& pop ) {
	m_pops.push_back( pop );
	m_game->RefreshBase( this );
}

const types::Buffer Base::Serialize( const Base* base ) {
	types::Buffer buf;
	buf.WriteInt( base->m_id );
	buf.WriteInt( base->m_owner->GetIndex() );
	buf.WriteString( base->m_faction->m_id );
	buf.WriteInt( base->m_tile->coord.x );
	buf.WriteInt( base->m_tile->coord.y );
	buf.WriteString( base->m_name );
	buf.WriteInt( base->m_pops.size() );
	for ( const auto& pop : base->m_pops ) {
		pop.Serialize( buf );
	}
	return buf;
}

Base* Base::Unserialize( types::Buffer& buf, Game* game ) {
	const auto id = buf.ReadInt();
	auto* slot = game
		? &game->GetState()->m_slots->GetSlot( buf.ReadInt() )
		: nullptr;
	const auto faction_id = buf.ReadString();
	auto* faction = game->GetFaction( faction_id );
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = game
		? game->GetMap()->GetTile( pos_x, pos_y )
		: nullptr;
	const auto name = buf.ReadString();
	pops_t pops = {};
	const auto pops_count = buf.ReadInt();
	pops.resize( pops_count );
	for ( auto& pop : pops ) {
		pop.Unserialize( buf, game );
	}
	return new Base( game, id, slot, faction, tile, name, pops );
}

WRAPIMPL_DYNAMIC_GETTERS( Base, CLASS_BASE )
	WRAPIMPL_GET( "id", Int, m_id )
	WRAPIMPL_LINK( "get_owner", m_owner )
	WRAPIMPL_LINK( "get_tile", m_tile )
	{
		"create_pop",
		NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( data, 0, Object );
		N_GETPROP( poptype, data, "type", String );
		auto* def = m_game->GetPopDef( poptype );
		if ( !def ) {
			GSE_ERROR( gse::EC.INVALID_DEFINITION, "Unknown pop type: " + poptype );
		}
		const auto max_variants = (m_faction->m_flags & Faction::FF_PROGENITOR)
			? 1 // aliens have 1 gender
			: 2; // humans have 2
		ASSERT_NOLOG( max_variants > 0, "no variants found for pop type: " + poptype );
		AddPop( Pop( this, def, m_game->GetRandom()->GetUInt(0, max_variants - 1) ) );
		return VALUE( gse::type::Undefined );
	} )
	},
WRAPIMPL_DYNAMIC_SETTERS( Base )
WRAPIMPL_DYNAMIC_ON_SET( Base )
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Base )

}
}
}
