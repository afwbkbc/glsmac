#pragma once

#include <vector>
#include <string>

#include "unit/Unit.h"
#include "Turn.h"

namespace game {

class BackendRequest {
public:

	enum request_type_t {
		BR_NONE,
		BR_ANIMATION_FINISHED,
	};
	BackendRequest( const request_type_t type );
	BackendRequest( const BackendRequest& other );
	virtual ~BackendRequest();

	const request_type_t type = BR_NONE;

	union {
		struct {
			size_t animation_id;
		} animation_finished;
	} data;
};

}
