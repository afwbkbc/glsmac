include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Needed for freetype

add_subdirectory(glew)
add_subdirectory(freetype)
add_subdirectory(SDL2_image)
add_subdirectory(SDL2)

