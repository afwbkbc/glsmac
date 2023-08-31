MESSAGE( STATUS "Fetching freetype ..." )

SET( FT_DISABLE_ZLIB ON )
SET( FT_DISABLE_BZIP2 ON )
SET( FT_DISABLE_PNG ON )
SET( FT_DISABLE_HARFBUZZ ON )
SET( FT_DISABLE_BROTLI ON )
SET( SKIP_INSTALL_ALL ON )

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG        VER-2-13-2
)

FETCHCONTENT_MAKEAVAILABLE( freetype )
