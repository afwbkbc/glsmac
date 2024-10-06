#pragma once

namespace game {
namespace backend {
namespace turn {

enum turn_status_t {
	TS_NONE,
	TS_PLEASE_WAIT,
	TS_TURN_ACTIVE,
	TS_TURN_COMPLETE,
	TS_WAITING_FOR_PLAYERS,
};

}
}
}
