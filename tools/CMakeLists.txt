CMAKE_MINIMUM_REQUIRED( VERSION 3.13 )

SET( CMAKE_MODULE_PATH "." )
SET( EXECUTABLE_OUTPUT_PATH "bin" )
ADD_EXECUTABLE( embedder embedder.cpp )
SET_PROPERTY(TARGET embedder PROPERTY CXX_STANDARD 17)
