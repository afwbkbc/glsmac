#pragma once

#include <vector>

#include "common/MTModule.h"

namespace network {

// this uniquely identifies connection
// auto-incremented to avoid rare race condition
// wrapped near UINT32_MAX
typedef uint32_t cid_t;
}

#include "Event.h"

namespace network {

enum op_t {
	OP_NONE,
	OP_SUCCESS, // just confirm success
	OP_CONNECT,
	OP_DISCONNECT,
	OP_DISCONNECT_CLIENT,
	OP_GETEVENTS,
	OP_SENDEVENT, // todo: multiple?
};

enum connection_mode_t {
	CM_NONE,
	CM_SERVER,
	CM_CLIENT,
};

enum result_t {
	R_NONE,
	R_SUCCESS,
	R_ERROR,
	R_CANCELED,
};

typedef std::vector< LegacyEvent > events_t;

struct MT_Request {
	op_t op;
	struct {
		connection_mode_t mode;
		std::string remote_address;
	} connect;
	cid_t cid;
	LegacyEvent event;
};

struct MT_Response {
	result_t result;
	std::string message;
	events_t events;
};

typedef common::MTModule< MT_Request, MT_Response > MTModule;

}
