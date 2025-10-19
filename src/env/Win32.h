#pragma once

#ifdef _WIN32
#undef UNICODE

#if !defined(ssize_t)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <WinSock2.h>

#define fd_t SOCKET
typedef long long ptr_int_t;

// static link glew, otherwise there are linker errors
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

// fix collisions with windows libraries
#undef CM_NONE
#undef RGB
#undef DELETE
#undef ERROR
#undef CALLBACK
#undef MB_RIGHT

#endif
