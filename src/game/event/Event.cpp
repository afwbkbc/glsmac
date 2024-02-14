#include "Event.h"

#include "SpawnUnit.h"

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
		case ET_SPAWN_UNIT: {
			SpawnUnit::Serialize( buf, (SpawnUnit*)event );
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
		case ET_SPAWN_UNIT:
			return SpawnUnit::Unserialize( buf );
		default:
			THROW( "unknown event type on read: " + std::to_string( type ) );
	}

}

}
}
