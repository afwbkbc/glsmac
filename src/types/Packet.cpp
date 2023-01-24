#include "Packet.h"

namespace types {

const Buffer Packet::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( type );
	
	switch ( type ) {
		case PT_AUTH: {
			buf.WriteString( data.str );
			break;
		}
		case PT_GLOBAL_SETTINGS: {
			buf.WriteString( data.str );
			break;
		}
		case PT_PLAYERS: {
			buf.WriteInt( data.vec.size() );
			for ( auto& s : data.vec ) {
				buf.WriteString( s );
			}
			break;
		}
	}
	
	return buf;
}

void Packet::Unserialize( Buffer buf ) {
	
	type = ( packet_type_t ) buf.ReadInt();
	
	switch ( type ) {
		case PT_AUTH: {
			data.str = buf.ReadString();
			break;
		}
		case PT_GLOBAL_SETTINGS: {
			data.str = buf.ReadString();
			break;
		}
		case PT_PLAYERS: {
			size_t count = buf.ReadInt();
			data.vec.clear();
			for ( size_t i = 0 ; i < count ; i++ ) {
				data.vec.push_back( buf.ReadString() );
			}
			break;
		}
	}
}


}