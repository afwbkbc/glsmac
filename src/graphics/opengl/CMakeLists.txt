SUBDIR( routine )
SUBDIR( shader_program )
SUBDIR( object )

SET(SRC ${SRC}

	${PWD}/Texture.cpp
	${PWD}/FontTexture.cpp
		${PWD}/CubemapTexture.cpp
	${PWD}/Actor.cpp
		${PWD}/MeshActor.cpp
		${PWD}/TextActor.cpp
		${PWD}/ImageActor.cpp
	${PWD}/Scene.cpp
	${PWD}/OpenGLGraphics.cpp

PARENT_SCOPE)
