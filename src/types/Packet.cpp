#include "Packet.h"

#include <limits>

namespace types {

Packet::Packet( const packet_type_t type )
	: type( type ) {

}

const types::Buffer Packet::Serialize() const {
	types::Buffer buf;

	if ( type <= PT_NONE || type >= PT_MAX ) {
		THROW( "invalid packet type " + std::to_string( type ) );
	}
	buf.WriteInt( type );

	switch ( type ) {
		case PT_REQUEST_AUTH:
		case PT_PING:
		case PT_PONG: {
			// no data
			break;
		}
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
		case PT_GAME_EVENT: {
			buf.WriteString( data.str ); // serialized game event
			break;
		}
		case PT_GAME_EVENT_RESPONSE: {
			buf.WriteString( data.str ); // event id
			buf.WriteBool( data.boolean ); // is accepted or not
			buf.WriteString( data.str2 );
			break;
		}
		default: {
			THROW( "invalid packet type " + std::to_string( type ) );
		}
	}

	return buf;
}

void Packet::Deserialize( types::Buffer buf ) {

	ASSERT( type == PT_NONE, "unserializing into existing packet" );

	const auto serialized_type = buf.ReadInt();
	if ( serialized_type <= PT_NONE || serialized_type >= PT_MAX ) {
		THROW( "invalid packet type " + std::to_string( serialized_type ) );
	}
	type = static_cast< packet_type_t >( serialized_type );

	switch ( type ) {
		case PT_REQUEST_AUTH:
		case PT_PING:
		case PT_PONG: {
			// no data
			break;
		}
		case PT_AUTH: {
			data.vec = {
				buf.ReadString(), // gsid
				buf.ReadString(), // player name
			};
			break;
		}
		case PT_PLAYERS: {
			data.num = buf.ReadInt< size_t >( "assigned player slot" );
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
			data.num = buf.ReadInt< size_t >( "player slot update index" );
			data.str = buf.ReadString(); // serialized slot
			break;
		}
		case PT_UPDATE_FLAGS: {
			udata.flags.flags = buf.ReadInt< size_t >( "player flags" );
			break;
		}
		case PT_FLAGS_UPDATE: {
			udata.flags.slot_num = buf.ReadInt< size_t >( "player flags slot" );
			udata.flags.flags = buf.ReadInt< size_t >( "player flags" );
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
			const auto state = buf.ReadInt();
			if ( state < 0 || state > std::numeric_limits< uint8_t >::max() ) {
				THROW( "invalid serialized game state " + std::to_string( state ) );
			}
			udata.game_state.state = static_cast< uint8_t >( state );
			break;
		}
		case PT_DOWNLOAD_REQUEST: {
			// no data
			break;
		}
		case PT_DOWNLOAD_RESPONSE: {
			data.num = buf.ReadInt< size_t >( "download size" );
			break;
		}
		case PT_DOWNLOAD_NEXT_CHUNK_REQUEST: {
			udata.download.offset = buf.ReadInt< size_t >( "download chunk offset" );
			udata.download.size = buf.ReadInt< size_t >( "download chunk size" );
			break;
		}
		case PT_DOWNLOAD_NEXT_CHUNK_RESPONSE: {
			udata.download.offset = buf.ReadInt< size_t >( "download chunk offset" );
			udata.download.size = buf.ReadInt< size_t >( "download chunk size" );
			data.str = buf.ReadString(); // serialized chunk
			break;
		}
		case PT_GAME_EVENT: {
			data.str = buf.ReadString(); // serialized game event
			break;
		}
		case PT_GAME_EVENT_RESPONSE: {
			data.str = buf.ReadString(); // event id
			data.boolean = buf.ReadBool(); // is accepted or not
			data.str2 = buf.ReadString(); // resolutions, if any
			break;
		}
		default: {
			THROW( "invalid packet type " + std::to_string( type ) );
		}
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected trailing packet data" );
	}
}

}
