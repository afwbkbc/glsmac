#include "Cache.h"

#include <iostream>

#include "scene/actor/Cache.h"
#include "common/ObjectLink.h"

namespace graphics {
namespace opengl {

Cache::Cache( OpenGL* opengl, scene::actor::Cache* actor )
	: Actor( opengl, actor ) {

}

Cache::~Cache() {

}

void Cache::DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	std::cout << "DRAW BEGIN: " << GetName() << std::endl;
	const auto& children = ( (scene::actor::Cache*)m_actor )->GetCacheChildren();
	for ( const auto& actor : children ) {
		ASSERT( actor->m_graphics_object, "cache child obj not set" );
		auto* obj = actor->m_graphics_object->GetDstObject< Actor >();
		ASSERT( obj->m_actor->GetScene() == m_actor->GetScene(), "child has different scene" );
		std::cout << "DRAW BEGIN: " << obj->GetName() << std::endl;
		obj->DrawImpl( shader_program, camera );
		const auto err = glGetError();
		ASSERT( !err, (std::string)"Cache child draw error: " + (char*)glewGetErrorString( err ) );
		std::cout << "DRAW END: " << obj->GetName() << std::endl;
	}
	std::cout << "DRAW END: " << GetName() << std::endl;
}

}
}