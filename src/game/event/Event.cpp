#include "Event.h"

#include "DefineUnit.h"
#include "SpawnUnit.h"
#include "DespawnUnit.h"
#include "MoveUnit.h"

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
	switch ( event->m_type ) {
		case ET_UNIT_DEFINE: {
			DefineUnit::Serialize( buf, (DefineUnit*)event );
			break;
		}
		case ET_UNIT_SPAWN: {
			SpawnUnit::Serialize( buf, (SpawnUnit*)event );
			break;
		}
		case ET_UNIT_DESPAWN: {
			DespawnUnit::Serialize( buf, (DespawnUnit*)event );
			break;
		}
		case ET_UNIT_MOVE: {
			MoveUnit::Serialize( buf, (MoveUnit*)event );
			break;
		}
		default:
			THROW( "unknown event type on write: " + std::to_string( event->m_type ) );
	}
	return buf;
}

Event* Event::Unserialize( types::Buffer& buf ) {
	const auto initiator_slot = buf.ReadInt();
	const auto type = buf.ReadInt();
	Event* result = nullptr;
	switch ( type ) {
		case ET_UNIT_DEFINE: {
			result = DefineUnit::Unserialize( buf, initiator_slot );
			break;
		}
		case ET_UNIT_SPAWN: {
			result = SpawnUnit::Unserialize( buf, initiator_slot );
			break;
		}
		case ET_UNIT_DESPAWN: {
			result = DespawnUnit::Unserialize( buf, initiator_slot );
			break;
		}
		case ET_UNIT_MOVE: {
			result = MoveUnit::Unserialize( buf, initiator_slot );
			break;
		}
		default:
			THROW( "unknown event type on read: " + std::to_string( type ) );
	}
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

}
}
