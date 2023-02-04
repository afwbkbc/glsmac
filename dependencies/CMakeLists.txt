include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Needed for freetype

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(SDL2)
add_subdirectory(SDL2_image)
add_subdirectory(glew)
add_subdirectory(freetype)

