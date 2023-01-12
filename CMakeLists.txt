cmake_minimum_required (VERSION 2.6)

SET(PROJECT_NAME OpenSMAC)
project (${PROJECT_NAME})

SET(CMAKE_MODULE_PATH ".")

SET(EXECUTABLE_OUTPUT_PATH "bin")

SET(SRC "")
SET(PWD ".")

FUNCTION(SUBDIR DIR)
	SET(PWD "${PWD}/${DIR}")
	add_subdirectory(${DIR})
	SET(SRC ${SRC} PARENT_SCOPE)
ENDFUNCTION(SUBDIR)

SUBDIR(src)
add_executable(${PROJECT_NAME}
	${SRC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O2 -ggdb -DDEBUG")

include_directories("src")

INCLUDE(FindPkgConfig)

#find_package(Boost REQUIRED COMPONENTS system thread atomic chrono)
#include_directories(${Boost_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})

#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

#find_package(SDL2IMAGE REQUIRED)
#include_directories(${SDL2IMAGE_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARIES})

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARY})

SET( CMAKE_CXX_FLAGS  " -std=c++17 ${CMAKE_CXX_FLAGS} " )
