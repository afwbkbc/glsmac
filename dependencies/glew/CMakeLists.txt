#GLEW <- DO NOT REMOVE; NEEDED FOR A TEST

cmake_minimum_required(VERSION 3.4)

project(fetch_content)

message(STATUS "Fetching GLEW ...")

include(FetchContent)
FetchContent_Declare(
  glew
  GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
  GIT_TAG        origin/master
)

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
  FetchContent_Populate(glew)

  add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if(WIN32)
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			set_target_properties(libglew_static PROPERTIES COMPILE_FLAGS "/MTd /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /GF /w34388 /w34389 /RTC1 /EHsc /Od /RTC1")
		else()
			set_target_properties(libglew_static PROPERTIES COMPILE_FLAGS "/MTd /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /Zc:preprocessor /GF /w34388 /w34389 /RTC1 /EHsc /Od /RTC1")
		endif()
	else()
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			set_target_properties(libglew_static PROPERTIES COMPILE_FLAGS "/MT /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /GF /w34388 /w34389 /wd4530 /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline")
		else()
			set_target_properties(libglew_static PROPERTIES COMPILE_FLAGS "/MT /wd4100 /wd4189 /wd4065 /GR- /W3 /permissive- /Zc:preprocessor /GF /w34388 /w34389 /wd4530 /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:inline")
		endif()
	endif()
endif()

add_executable(glewinfo ${glew_SOURCE_DIR}/src/glewinfo.c)
target_link_libraries(glewinfo PRIVATE libglew_static)
