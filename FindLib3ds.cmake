# - Try to find lib3ds
# Once done this will define
#
# LIB3DS_FOUND - system has lib3ds
# LIB3DS_INCLUDE_DIR - the lib3ds include directory
# LIB3DS_LIB - link these to use lib3ds

find_path(LIB3DS_INCLUDE_DIR
	HINTS
	NAMES lib3ds.h
	PATHS /usr/include
		$ENV{LIB3DSDIR}/include
		/usr/local/include
		$ENV{INCLUDE}
)

find_library(LIB3DS_LIB
	HINTS
	PATHS /usr/lib
		$ENV{LIB3DSDIR}/lib
	NAMES 3ds
		lib3ds
)

if(LIB3DS_INCLUDE_DIR)
	message(STATUS "Found lib3ds include dir: ${LIB3DS_INCLUDE_DIR}")
else(LIB3DS_INCLUDE_DIR)
	message(STATUS "Could NOT find lib3ds headers.")
endif(LIB3DS_INCLUDE_DIR)

if(LIB3DS_LIB)
	message(STATUS "Found lib3ds library: ${LIB3DS_LIB}")
else(LIB3DS_LIB)
	message(STATUS "Could NOT find lib3ds library.")
endif(LIB3DS_LIB)

if(LIB3DS_INCLUDE_DIR AND LIB3DS_LIB)
	set(LIB3DS_FOUND TRUE)
else(LIB3DS_INCLUDE_DIR AND LIB3DS_LIB)
	set(LIB3DS_FOUND FALSE)
	if(Lib3ds_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find lib3ds. Please install lib3ds (http://lib3ds.sourceforge.net)")
	endif(Lib3ds_FIND_REQUIRED)
endif(LIB3DS_INCLUDE_DIR AND LIB3DS_LIB)
