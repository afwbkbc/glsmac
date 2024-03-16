#include "Event.h"

#include "DefineUnit.h"
#include "SpawnUnit.h"
#include "DespawnUnit.h"
#include "MoveUnit.h"
#include "SkipUnitTurn.h"
#include "CompleteTurn.h"
#include "UncompleteTurn.h"

namespace game {
namespace event {

Event::Event( const size_t initiator_slot, const event_type_t type )
	: m_initiator_slot( initiator_slot )
	, m_type( type ) {
	//
}

const types::Buffer Event::Serialize( const Event* event ) {
	types::Buffer buf;
	buf.WriteInt( event->m_initiator_slot );
	buf.WriteInt( event->m_type );
#define SERIALIZE( _type, _class ) \
    case _type: { \
        _class::Serialize( buf, (_class*)event ); \
        break; \
    }
	switch ( event->m_type ) {
		SERIALIZE( ET_UNIT_DEFINE, DefineUnit )
		SERIALIZE( ET_UNIT_SPAWN, SpawnUnit )
		SERIALIZE( ET_UNIT_DESPAWN, DespawnUnit )
		SERIALIZE( ET_UNIT_MOVE, MoveUnit )
		SERIALIZE( ET_UNIT_SKIP_TURN, SkipUnitTurn )
		SERIALIZE( ET_COMPLETE_TURN, CompleteTurn )
		SERIALIZE( ET_UNCOMPLETE_TURN, UncompleteTurn )
		default:
			THROW( "unknown event type on write: " + std::to_string( event->m_type ) );
	}
#undef SERIALIZE
	return buf;
}

Event* Event::Unserialize( types::Buffer& buf ) {
	const auto initiator_slot = buf.ReadInt();
	const auto type = buf.ReadInt();
	Event* result = nullptr;
#define UNSERIALIZE( _type, _class ) \
    case _type: { \
        result = _class::Unserialize( buf, initiator_slot ); \
        break; \
    }
	switch ( type ) {
		UNSERIALIZE( ET_UNIT_DEFINE, DefineUnit )
		UNSERIALIZE( ET_UNIT_SPAWN, SpawnUnit )
		UNSERIALIZE( ET_UNIT_DESPAWN, DespawnUnit )
		UNSERIALIZE( ET_UNIT_MOVE, MoveUnit )
		UNSERIALIZE( ET_UNIT_SKIP_TURN, SkipUnitTurn )
		UNSERIALIZE( ET_COMPLETE_TURN, CompleteTurn )
		UNSERIALIZE( ET_UNCOMPLETE_TURN, UncompleteTurn )
		default:
			THROW( "unknown event type on read: " + std::to_string( type ) );
	}
#undef UNSERIALIZE
	return result;
}

const types::Buffer Event::SerializeMultiple( const std::vector< Event* >& events ) {
	types::Buffer buf;
	buf.WriteInt( events.size() );
	for ( const auto& event : events ) {
		buf.WriteString( game::event::Event::Serialize( event ).ToString() );
	}
	const auto serialized_events = buf.ToString();
	return buf;
}

void Event::UnserializeMultiple( types::Buffer& buf, std::vector< Event* >& events_out ) {
	const auto count = buf.ReadInt();
	for ( auto i = 0 ; i < count ; i++ ) {
		auto event_buf = types::Buffer( buf.ReadString() );
		events_out.push_back( game::event::Event::Unserialize( event_buf ) );
	}
}

const std::string* Event::Ok() const {
	return nullptr;
}

const std::string* Event::Error( const std::string& text ) const {
	return new std::string( text );
}

}
}
