#GLEW <- DO NOT REMOVE; NEEDED FOR A TEST

PROJECT( fetch_content )

MESSAGE( STATUS "Fetching GLEW ..." )

INCLUDE( FetchContent )
FETCHCONTENT_DECLARE(
	glew
	GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
	GIT_TAG glew-cmake-2.3.1
)

FETCHCONTENT_GETPROPERTIES( glew )
IF ( NOT glew_POPULATED )
	FETCHCONTENT_MAKEAVAILABLE( glew )
ENDIF ()
