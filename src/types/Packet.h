#pragma once

#include <vector>

#include "Serializable.h"

namespace types {

CLASS( Packet, Serializable )

	enum packet_type_t : uint8_t {
		PT_NONE, // -
		PT_REQUEST_AUTH, // S->C
		PT_AUTH, // C->S
		PT_PING, // *->*
		PT_PONG, // *->*

		PT_PLAYERS, // S->C
		PT_GLOBAL_SETTINGS, // S->C
		PT_UPDATE_SLOT, // C->S
		PT_SLOT_UPDATE, // S->C
		PT_UPDATE_FLAGS, // C->S
		PT_FLAGS_UPDATE, // S->C
		PT_KICK, // S->C
		PT_MESSAGE, // *->*
		PT_GAME_STATE, // S->C
		PT_DOWNLOAD_REQUEST, // C->S
		PT_DOWNLOAD_RESPONSE, // S->C
		PT_DOWNLOAD_NEXT_CHUNK_REQUEST, // C->S
		PT_DOWNLOAD_NEXT_CHUNK_RESPONSE, // S->C
		PT_GAME_EVENT, // *->*
		PT_GAME_EVENT_RESPONSE, // S->C
	};

	Packet( const packet_type_t type );

	packet_type_t type;

	union {
		time_t time;
		struct {
			size_t id;
		} ping;
		struct {
			size_t slot_num;
			size_t flags;
		} flags;
		struct {
			uint8_t state;
		} game_state;
		struct {
			size_t offset;
			size_t size;
		} download;
	} udata;

	// TODO: move this into udata
	struct {
		bool boolean;
		size_t num;
		std::string str;
		std::string str2;
		std::vector< std::string > vec;
	} data;

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buffer ) override;
};

}
