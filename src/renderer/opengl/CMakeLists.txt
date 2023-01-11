SUBDIR( routine )
SUBDIR( shader_program )
SUBDIR( object )

SET(SRC ${SRC}

	${PWD}/Font.cpp
	${PWD}/Texture.cpp
		${PWD}/CubemapTexture.cpp
	${PWD}/Actor.cpp
		${PWD}/MeshActor.cpp
		${PWD}/TextActor.cpp
		${PWD}/ImageActor.cpp
	${PWD}/Scene.cpp
	${PWD}/OpenGLRenderer.cpp

PARENT_SCOPE)
