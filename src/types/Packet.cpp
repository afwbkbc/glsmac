#include "Packet.h"

namespace types {

Packet::Packet( const packet_type_t type )
	: type( type ) {

}

const Buffer Packet::Serialize() const {
	Buffer buf;

	buf.WriteInt( type );

	switch ( type ) {
		case PT_AUTH: {
			buf.WriteString( data.str ); // player name
			break;
		}
		case PT_GLOBAL_SETTINGS: {
			buf.WriteString( data.str ); // serialized settings
			break;
		}
		case PT_PLAYERS: {
			buf.WriteInt( data.num ); // assigned slot num
			buf.WriteString( data.str ); // serialized slots
			break;
		}
		case PT_UPDATE_SLOT: {
			buf.WriteString( data.str ); // serialized slot
			break;
		}
		case PT_SLOT_UPDATE: {
			buf.WriteInt( data.num ); // player slot num
			buf.WriteString( data.str ); // serialized slot
			break;
		}
		case PT_KICK: {
			buf.WriteString( data.str ); // reason
			break;
		}
		case PT_MESSAGE: {
			buf.WriteString( data.str ); // message
			break;
		}
		case PT_TILES: {
			buf.WriteBool( data.boolean ); // is map ready?
			if ( !data.boolean ) {
				buf.WriteBool( data.num ); // percents done
			}
			else {
				buf.WriteString( data.str ); // serialized tiles
			}
			break;
		}
		default: {
			//ASSERT(false, "unknown packet type " + std::to_string( type ));
		}
	}

	return buf;
}

void Packet::Unserialize( Buffer buf ) {

	ASSERT( type == PT_NONE, "unserializing into existing packet" );

	type = (packet_type_t)buf.ReadInt();

	switch ( type ) {
		case PT_AUTH: {
			data.str = buf.ReadString(); // player name
			break;
		}
		case PT_GLOBAL_SETTINGS: {
			data.str = buf.ReadString(); // serialized settings
			break;
		}
		case PT_PLAYERS: {
			data.num = buf.ReadInt(); // assigned slot num
			data.str = buf.ReadString(); // serialized slots
			break;
		}
		case PT_UPDATE_SLOT: {
			data.str = buf.ReadString(); // serialized slot
			break;
		}
		case PT_SLOT_UPDATE: {
			data.num = buf.ReadInt(); // player slot num
			data.str = buf.ReadString(); // serialized slot
			break;
		}
		case PT_KICK: {
			data.str = buf.ReadString(); // reason
			break;
		}
		case PT_MESSAGE: {
			data.str = buf.ReadString(); // message
			break;
		}
		case PT_TILES: {
			data.boolean = buf.ReadBool(); // is map ready?
			if ( !data.boolean ) {
				data.num = buf.ReadBool(); // percents done
			}
			else {
				data.str = buf.ReadString(); // serialized tiles
			}
			break;
		}
		default: {
			//ASSERT(false, "unknown packet type " + std::to_string(type));
		}
	}
}

}