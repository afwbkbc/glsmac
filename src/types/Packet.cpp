#include "Packet.h"

namespace types {

Packet::Packet( const packet_type_t type )
	: type( type ) {

}

const types::Buffer Packet::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( type );

	switch ( type ) {
		case PT_AUTH: {
			buf.WriteString( data.vec[ 0 ] ); // gsid
			buf.WriteString( data.vec[ 1 ] ); // player name
			break;
		}
		case PT_PLAYERS: {
			buf.WriteInt( data.num ); // assigned slot num
			buf.WriteString( data.str ); // serialized slots
			break;
		}
		case PT_GLOBAL_SETTINGS: {
			buf.WriteString( data.str ); // serialized settings
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
		case PT_UPDATE_FLAGS: {
			buf.WriteInt( udata.flags.flags );
			break;
		}
		case PT_FLAGS_UPDATE: {
			buf.WriteInt( udata.flags.slot_num );
			buf.WriteInt( udata.flags.flags );
		}
		case PT_KICK: {
			buf.WriteString( data.str ); // reason
			break;
		}
		case PT_MESSAGE: {
			buf.WriteString( data.str ); // message
			break;
		}
		case PT_GAME_STATE: {
			buf.WriteInt( udata.game_state.state );
			break;
		}
		case PT_DOWNLOAD_REQUEST: {
			// no data
			break;
		}
		case PT_DOWNLOAD_RESPONSE: {
			buf.WriteInt( data.num ); // total size of serialized data
			break;
		}
		case PT_DOWNLOAD_NEXT_CHUNK_REQUEST: {
			buf.WriteInt( udata.download.offset );
			buf.WriteInt( udata.download.size );
			break;
		}
		case PT_DOWNLOAD_NEXT_CHUNK_RESPONSE: {
			buf.WriteInt( udata.download.offset );
			buf.WriteInt( udata.download.size );
			buf.WriteString( data.str ); // serialized chunk
			break;
		}
		case PT_GAME_EVENTS: {
			buf.WriteString( data.str ); // serialized game events
			break;
		}
		default: {
			//ASSERT(false, "unknown packet type " + std::to_string( type ));
		}
	}

	return buf;
}

void Packet::Unserialize( types::Buffer buf ) {

	ASSERT( type == PT_NONE, "unserializing into existing packet" );

	type = (packet_type_t)buf.ReadInt();

	switch ( type ) {
		case PT_AUTH: {
			data.vec = {
				buf.ReadString(), // gsid
				buf.ReadString(), // player name
			};
			break;
		}
		case PT_PLAYERS: {
			data.num = buf.ReadInt(); // assigned slot num
			data.str = buf.ReadString(); // serialized slots
			break;
		}
		case PT_GLOBAL_SETTINGS: {
			data.str = buf.ReadString(); // serialized settings
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
		case PT_UPDATE_FLAGS: {
			udata.flags.flags = buf.ReadInt();
			break;
		}
		case PT_FLAGS_UPDATE: {
			udata.flags.slot_num = buf.ReadInt();
			udata.flags.flags = buf.ReadInt();
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
		case PT_GAME_STATE: {
			udata.game_state.state = buf.ReadInt();
			break;
		}
		case PT_DOWNLOAD_REQUEST: {
			// no data
			break;
		}
		case PT_DOWNLOAD_RESPONSE: {
			data.num = buf.ReadInt(); // total size of serialized data
			break;
		}
		case PT_DOWNLOAD_NEXT_CHUNK_REQUEST: {
			udata.download.offset = buf.ReadInt();
			udata.download.size = buf.ReadInt();
			break;
		}
		case PT_DOWNLOAD_NEXT_CHUNK_RESPONSE: {
			udata.download.offset = buf.ReadInt();
			udata.download.size = buf.ReadInt();
			data.str = buf.ReadString(); // serialized chunk
			break;
		}
		case PT_GAME_EVENTS: {
			data.str = buf.ReadString(); // serialized game events
			break;
		}
		default: {
			//ASSERT(false, "unknown packet type " + std::to_string(type));
		}
	}
}

}