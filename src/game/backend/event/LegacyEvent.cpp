#include "LegacyEvent.h"

#include "DefineResource.h"
#include "DefineAnimation.h"
#include "DefineMorales.h"
#include "DefineUnit.h"
#include "SpawnUnit.h"
#include "DespawnUnit.h"
#include "MoveUnit.h"
#include "AttackUnit.h"
#include "SkipUnitTurn.h"
#include "DefinePop.h"
#include "SpawnBase.h"
#include "CompleteTurn.h"
#include "UncompleteTurn.h"
#include "FinalizeTurn.h"
#include "TurnFinalized.h"
#include "AdvanceTurn.h"
#include "RequestTileLocks.h"
#include "LockTiles.h"
#include "RequestTileUnlocks.h"
#include "UnlockTiles.h"

namespace game {
namespace backend {
namespace event {

LegacyEvent::LegacyEvent( const size_t initiator_slot, const event_type_t type )
	: m_initiator_slot( initiator_slot )
	, m_type( type ) {
	//
}

const types::Buffer LegacyEvent::Serialize( const LegacyEvent* event ) {
	types::Buffer buf;
	buf.WriteInt( event->m_initiator_slot );
	buf.WriteInt( event->m_type );
#define SERIALIZE( _type, _class ) \
    case _type: { \
        _class::Serialize( buf, (_class*)event ); \
        break; \
    }
	switch ( event->m_type ) {
		SERIALIZE( ET_RESOURCE_DEFINE, DefineResource )
		SERIALIZE( ET_ANIMATION_DEFINE, DefineAnimation )
		SERIALIZE( ET_UNIT_DEFINE_MORALES, DefineMorales )
		SERIALIZE( ET_UNIT_DEFINE, DefineUnit )
		SERIALIZE( ET_UNIT_SPAWN, SpawnUnit )
		SERIALIZE( ET_UNIT_DESPAWN, DespawnUnit )
		SERIALIZE( ET_UNIT_MOVE, MoveUnit )
		SERIALIZE( ET_UNIT_ATTACK, AttackUnit )
		SERIALIZE( ET_UNIT_SKIP_TURN, SkipUnitTurn )
		SERIALIZE( ET_BASE_DEFINE_POP, DefinePop )
		SERIALIZE( ET_BASE_SPAWN, SpawnBase )
		SERIALIZE( ET_COMPLETE_TURN, CompleteTurn )
		SERIALIZE( ET_UNCOMPLETE_TURN, UncompleteTurn )
		SERIALIZE( ET_FINALIZE_TURN, FinalizeTurn )
		SERIALIZE( ET_TURN_FINALIZED, TurnFinalized )
		SERIALIZE( ET_ADVANCE_TURN, AdvanceTurn )
		SERIALIZE( ET_REQUEST_TILE_LOCKS, RequestTileLocks )
		SERIALIZE( ET_LOCK_TILES, LockTiles )
		SERIALIZE( ET_REQUEST_TILE_UNLOCKS, RequestTileUnlocks )
		SERIALIZE( ET_UNLOCK_TILES, UnlockTiles )
		default:
			THROW( "unknown event type on write: " + std::to_string( event->m_type ) );
	}
#undef SERIALIZE
	return buf;
}

LegacyEvent* LegacyEvent::Unserialize( GSE_CALLABLE, types::Buffer& buf ) {
	const auto initiator_slot = buf.ReadInt();
	const auto type = buf.ReadInt();
	LegacyEvent* result = nullptr;
#define UNSERIALIZE( _type, _class, ... ) \
    case _type: { \
        result = _class::Unserialize( __VA_ARGS__ buf, initiator_slot ); \
        break; \
    }
	switch ( type ) {
		UNSERIALIZE( ET_RESOURCE_DEFINE, DefineResource )
		UNSERIALIZE( ET_ANIMATION_DEFINE, DefineAnimation )
		UNSERIALIZE( ET_UNIT_DEFINE_MORALES, DefineMorales )
		UNSERIALIZE( ET_UNIT_DEFINE, DefineUnit )
		UNSERIALIZE( ET_UNIT_SPAWN, SpawnUnit )
		UNSERIALIZE( ET_UNIT_DESPAWN, DespawnUnit )
		UNSERIALIZE( ET_UNIT_MOVE, MoveUnit, GSE_CALL, )
		UNSERIALIZE( ET_UNIT_ATTACK, AttackUnit, GSE_CALL, )
		UNSERIALIZE( ET_UNIT_SKIP_TURN, SkipUnitTurn )
		UNSERIALIZE( ET_BASE_DEFINE_POP, DefinePop )
		UNSERIALIZE( ET_BASE_SPAWN, SpawnBase )
		UNSERIALIZE( ET_COMPLETE_TURN, CompleteTurn )
		UNSERIALIZE( ET_UNCOMPLETE_TURN, UncompleteTurn )
		UNSERIALIZE( ET_FINALIZE_TURN, FinalizeTurn )
		UNSERIALIZE( ET_TURN_FINALIZED, TurnFinalized )
		UNSERIALIZE( ET_ADVANCE_TURN, AdvanceTurn )
		UNSERIALIZE( ET_REQUEST_TILE_LOCKS, RequestTileLocks )
		UNSERIALIZE( ET_LOCK_TILES, LockTiles )
		UNSERIALIZE( ET_REQUEST_TILE_UNLOCKS, RequestTileUnlocks )
		UNSERIALIZE( ET_UNLOCK_TILES, UnlockTiles )
		default:
			THROW( "unknown event type on read: " + std::to_string( type ) );
	}
#undef UNSERIALIZE
	return result;
}

const types::Buffer LegacyEvent::SerializeMultiple( const std::vector< LegacyEvent* >& events ) {
	types::Buffer buf;
	buf.WriteInt( events.size() );
	for ( const auto& event : events ) {
		buf.WriteString( backend::event::LegacyEvent::Serialize( event ).ToString() );
	}
	const auto serialized_events = buf.ToString();
	return buf;
}

void LegacyEvent::UnserializeMultiple( GSE_CALLABLE, types::Buffer& buf, std::vector< LegacyEvent* >& events_out ) {
	const auto count = buf.ReadInt();
	for ( auto i = 0 ; i < count ; i++ ) {
		auto event_buf = types::Buffer( buf.ReadString() );
		events_out.push_back( backend::event::LegacyEvent::Unserialize( GSE_CALL, event_buf ) );
	}
}

const bool LegacyEvent::IsBroadcastable( const event_type_t type ) {
	switch ( type ) {
		case ET_REQUEST_TILE_LOCKS:
		case ET_REQUEST_TILE_UNLOCKS:
			return false;
		default:
			return true;
	}
}

void LegacyEvent::SetDestinationSlot( const uint8_t destination_slot ) {
	m_is_public = false;
	m_destination_slot = destination_slot;
}

const bool LegacyEvent::IsProcessableBy( const uint8_t destination_slot ) const {
	return m_is_public || m_destination_slot == destination_slot;
};

const bool LegacyEvent::IsSendableTo( const uint8_t destination_slot ) const {
	return IsBroadcastable( m_type ) && IsProcessableBy( destination_slot );
}

const std::string* LegacyEvent::Ok() const {
	return nullptr;
}

const std::string* LegacyEvent::Error( const std::string& text ) const {
	return new std::string( text );
}

}
}
}
