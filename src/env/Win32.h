#pragma once

#ifdef _WIN32

#ifdef VISUAL_STUDIO
	#if !defined(ssize_t)
		#include <BaseTsd.h>
		typedef SSIZE_T ssize_t;
	#endif
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

// static link glew, otherwise there are linker errors
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

// fix collisions with windows libraries
#undef CM_NONE
#undef RGB
#undef DELETE

#endif
