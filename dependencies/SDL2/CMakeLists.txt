message(STATUS "Fetching SDL2...")

set(SDL_PTHREADS ON)
add_compile_definitions(SDL_THREAD_GENERIC_COND_SUFFIX)

FetchContent_Declare(SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.26.2
)

FetchContent_MakeAvailable(SDL2)
