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
		if ( m_original_mesh ) {
			DELETE( m_original_mesh );
		}
		m_original_mesh = mesh;
		NEW( m_mesh, types::mesh::Mesh, *m_original_mesh ); // copy
		NEW( m_actor, scene::actor::Mesh, "UI::Mesh", m_mesh );
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
	if ( m_original_mesh ) {
		DELETE( m_original_mesh );
		m_original_mesh = nullptr;
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

void Mesh::Align() {
	
	Log( "MESH ALIGN" );
	
/*
	const Vec2<coord_t> v1 = { ClampX( m_object_area.left ), ClampY( m_object_area.top ) };
	const Vec2<coord_t> v2 = { ClampX( m_object_area.right ), ClampY( m_object_area.bottom ) };
	
	if ( m_texture && !m_stretch_texture ) {
		m_background_mesh->SetCoords( v1, v2, { m_texture->m_width, m_texture->m_height }, -m_z_index );
	}
	else {
		m_background_mesh->SetCoords( v1, v2, -m_z_index );
	}
*/	
}

}
}
