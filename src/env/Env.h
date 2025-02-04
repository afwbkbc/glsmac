#pragma once

#ifdef _WIN32
#include "Win32.h"
#else
#include "Posix.h"
#endif

#ifdef DEBUG
#include "Debug.h"
#else
#include "Release.h"
#endif
