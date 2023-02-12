#include "Mesh.h"

using namespace types;

namespace ui {
namespace object {

Mesh::Mesh( const std::string& class_name )
	: UIObject( class_name )
{
	//
}

void Mesh::SetMesh( const types::mesh::Mesh* mesh ) {
	if ( m_actor ) {
		RemoveActor( m_actor );
		DELETE( m_actor );
		m_actor = nullptr;
	}
	if ( mesh ) {
		NEW( m_actor, scene::actor::Mesh, "UI::Mesh", mesh );
		if ( m_texture ) {
			m_actor->SetTexture( m_texture );
		}
		AddActor( m_actor );
		Realign();
	}
}

void Mesh::SetTexture( const types::Texture* texture ) {
	if ( texture != m_texture ) {
		//Log( "Setting texture " + texture->m_name );
		m_texture = texture;
		if ( m_actor ) {
			m_actor->SetTexture( m_texture );
		}
		Realign();
	}
}

void Mesh::ClearTexture() {
	if ( m_texture ) {
		//Log( "Clearing texture" );
		if ( m_actor ) {
			m_actor->SetTexture( nullptr );
		}
		m_texture = nullptr;
	}
}

void Mesh::Destroy() {
	if ( m_actor ) {
		RemoveActor( m_actor );
		DELETE( m_actor );
	}

	UIObject::Destroy();
}

void Mesh::Draw() {
	UIObject::Draw();

	// looks like this isn't needed. keep this code for a while in case some bugs happen without it
	/*if ( m_texture ) {
		m_actor->SetTexture( m_texture );
	}*/
}

}
}
