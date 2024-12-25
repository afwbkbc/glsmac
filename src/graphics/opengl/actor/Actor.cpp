#include "Actor.h"

#include "scene/actor/Actor.h"
#include "Cache.h"

namespace graphics {
namespace opengl {

Actor::Actor( OpenGL* opengl, scene::actor::Actor* actor )
	: m_opengl( opengl )
	, m_actor( actor ) {
	m_name = actor->GetLocalName();
}

Actor::~Actor() {
	if ( m_cache_parent ) {
		m_cache_parent->RemoveCacheChild( this );
	}
}

void Actor::Draw( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	if ( !m_cache_parent ) { // otherwise cache parent will initiate draws when needed
		DrawImpl( shader_program, camera );
	}
}

const float Actor::GetZIndex() const {
	return m_z_index;
}

void Actor::SetZIndex( const float z_index ) {
	if ( m_cache_parent ) {
		m_cache_parent->SetCacheChildZIndex( this, z_index );
	}
	m_z_index = z_index;
}

void Actor::SetCacheParent( Cache* cache_parent ) {
	ASSERT( !m_cache_parent, "cache parent already set" );
	m_cache_parent = cache_parent;
	m_cache_parent->AddCacheChild( this );
}

}
}
