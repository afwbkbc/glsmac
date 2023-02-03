message(STATUS "Fetching SDL_image...")

FetchContent_Declare(sdl_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
    GIT_TAG release-2.6.2
)

FetchContent_MakeAvailable(sdl_image)