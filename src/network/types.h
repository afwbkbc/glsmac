#pragma once

namespace network {

// network socket identifier (differs between platforms)
#ifdef _WIN32
#include <WinSock2.h>
#define fd_t SOCKET
#else
#define fd_t int
#endif

// this uniquely identifies connection
// auto-incremented to avoid rare race condition
// wrapped near UINT32_MAX
typedef uint32_t cid_t;

}
