#pragma once

#ifdef _WIN32

// static link glew, otherwise there are linker errors
#define GLEW_STATIC

// fix collisions with windows libraries
#undef CM_NONE
#undef DELETE

#endif
