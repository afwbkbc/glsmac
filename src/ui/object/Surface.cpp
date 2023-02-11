#include "Surface.h"

using namespace types;

namespace ui {
namespace object {

Surface::Surface( const std::string& class_name )
	: Mesh( class_name )
{
	//
}

void Surface::SetStretchTexture( const bool stretch_texture ) {
	ASSERT( m_texture, "stretch texture but texture not set" );
	if ( stretch_texture != m_stretch_texture ) {
		m_stretch_texture = stretch_texture;
		Realign();
	}
}

void Surface::Create() {
	Mesh::Create();

	NEW( m_background_mesh, types::mesh::Rectangle );
	Resize();
	SetMesh( m_background_mesh );
}

void Surface::ApplyStyle() {
	Mesh::ApplyStyle();
	
	if ( Has( Style::A_TEXTURE ) ) {
		const auto* texture = (Texture*)GetObject( Style::A_TEXTURE );
		if ( texture ) {
			SetTexture( texture );
			if ( Has( Style::A_SIZE_FROM_TEXTURE ) ) {
				SetWidth( texture->m_width );
				SetHeight( texture->m_height );
			}
			if ( Has( Style::A_STRETCH_TEXTURE ) ) {
				SetStretchTexture( true );
			}
			if ( Has( Style::A_KEEP_TEXTURE_ASPECT_RATIO ) ) {
				ForceAspectRatio( (float) texture->m_height / texture->m_width );
			}
		}
		else {
			ClearTexture();
		}
	}
	else {
		ClearTexture();
	}
}

void Surface::Align() {
	Resize();
	
	Mesh::Align();
}

void Surface::Resize() {

	Vec2< coord_t > v1 = { ClampX( m_object_area.left ), ClampY( m_object_area.top ) };
	Vec2< coord_t > v2 = { ClampX( m_object_area.right ), ClampY( m_object_area.bottom ) };
	
	if ( m_texture && !m_stretch_texture ) {
		m_background_mesh->SetCoords( v1, v2, { m_texture->m_width, m_texture->m_height }, -m_z_index );
	}
	else {
		m_background_mesh->SetCoords( v1, v2, -m_z_index );
	}

}

}
}
