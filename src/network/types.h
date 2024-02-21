#pragma once

namespace network {

// this uniquely identifies connection
// auto-incremented to avoid rare race condition
// wrapped near UINT32_MAX
typedef uint32_t cid_t;

}
