#pragma once

#include "base/Base.h"

namespace network {
	
CLASS( Event, base::Base )
	
	enum event_type_t {
		ET_NONE,
		ET_CONNECT,
		ET_CLIENT_CONNECT,
		ET_DISCONNECT,
		ET_CLIENT_DISCONNECT,
		ET_PACKET,
	};
	
	size_t cid = 0; // 'client id', linked to network connection (usually to socket fd)
	
	event_type_t type = ET_NONE;
	struct {
		string remote_address;
		string packet_data;
	} data;
	
	void operator= ( const Event& other ) {
		cid = other.cid;
		type = other.type;
		data.remote_address = other.data.remote_address;
		data.packet_data = other.data.packet_data;
	}
	
	void Clear() {
		cid = 0;
		type = ET_NONE;
		data.remote_address.clear();
		data.packet_data.clear();
	}
	
};
	
}
