#include "Event.h"

#include "DefineUnit.h"
#include "SpawnUnit.h"
#include "DespawnUnit.h"

namespace game {
namespace event {

Event::Event( const event_type_t type )
	: m_type( type ) {
	//
}

const types::Buffer Event::Serialize( const Event* event ) {
	types::Buffer buf;
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
		default:
			THROW( "unknown event type on write: " + std::to_string( event->m_type ) );
	}
	return buf;
}

Event* Event::Unserialize( types::Buffer& buf ) {
	const auto type = buf.ReadInt();
	switch ( type ) {
		case ET_UNIT_DEFINE:
			return DefineUnit::Unserialize( buf );
		case ET_UNIT_SPAWN:
			return SpawnUnit::Unserialize( buf );
		case ET_UNIT_DESPAWN:
			return DespawnUnit::Unserialize( buf );
		default:
			THROW( "unknown event type on read: " + std::to_string( type ) );
	}

}

const types::Buffer Event::SerializeMultiple( const std::vector< const Event* >& events ) {
	types::Buffer buf;
	buf.WriteInt( events.size() );
	for ( const auto& event : events ) {
		buf.WriteString( game::event::Event::Serialize( event ).ToString() );
	}
	const auto serialized_events = buf.ToString();
	return buf;
}

void Event::UnserializeMultiple( types::Buffer& buf, std::vector< const Event* >& events_out ) {
	const auto count = buf.ReadInt();
	for ( auto i = 0 ; i < count ; i++ ) {
		auto event_buf = types::Buffer( buf.ReadString() );
		events_out.push_back( game::event::Event::Unserialize( event_buf ) );
	}
}

}
}
