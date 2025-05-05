#include "LegacyEvent.h"

#include "DefinePop.h"
#include "SpawnBase.h"
#include "CompleteTurn.h"
#include "UncompleteTurn.h"
#include "FinalizeTurn.h"
#include "TurnFinalized.h"
#include "AdvanceTurn.h"

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
		SERIALIZE( ET_BASE_DEFINE_POP, DefinePop )
		SERIALIZE( ET_BASE_SPAWN, SpawnBase )
		SERIALIZE( ET_COMPLETE_TURN, CompleteTurn )
		SERIALIZE( ET_UNCOMPLETE_TURN, UncompleteTurn )
		SERIALIZE( ET_FINALIZE_TURN, FinalizeTurn )
		SERIALIZE( ET_TURN_FINALIZED, TurnFinalized )
		SERIALIZE( ET_ADVANCE_TURN, AdvanceTurn )
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
		UNSERIALIZE( ET_BASE_DEFINE_POP, DefinePop )
		UNSERIALIZE( ET_BASE_SPAWN, SpawnBase )
		UNSERIALIZE( ET_COMPLETE_TURN, CompleteTurn )
		UNSERIALIZE( ET_UNCOMPLETE_TURN, UncompleteTurn )
		UNSERIALIZE( ET_FINALIZE_TURN, FinalizeTurn )
		UNSERIALIZE( ET_TURN_FINALIZED, TurnFinalized )
		UNSERIALIZE( ET_ADVANCE_TURN, AdvanceTurn )
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

void LegacyEvent::SetDestinationSlot( const uint8_t destination_slot ) {
	m_is_public = false;
	m_destination_slot = destination_slot;
}

const bool LegacyEvent::IsProcessableBy( const uint8_t destination_slot ) const {
	return m_is_public || m_destination_slot == destination_slot;
};

const bool LegacyEvent::IsSendableTo( const uint8_t destination_slot ) const {
	return IsProcessableBy( destination_slot );
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
