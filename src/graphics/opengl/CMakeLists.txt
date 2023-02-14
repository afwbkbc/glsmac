SUBDIR( routine )
SUBDIR( shader_program )
SUBDIR( object )
SUBDIR( actor )
SUBDIR( texture )

SET(SRC ${SRC}

	${PWD}/Scene.cpp
	${PWD}/FBO.cpp
	${PWD}/OpenGL.cpp

PARENT_SCOPE)
