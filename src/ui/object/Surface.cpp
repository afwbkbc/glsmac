#include "Surface.h"

using namespace types;

namespace ui {
namespace object {

Surface::Surface( const string& class_name )
	: UIObject( class_name )
{
	//
}

void Surface::SetTexture( const types::Texture* texture ) {
	if ( texture != m_texture ) {
		//Log( "Setting texture " + texture->m_name );
		m_texture = texture;
		if ( m_background ) {
			m_background->SetTexture( m_texture );
		}
		Realign();
	}
}

void Surface::ClearTexture() {
	if ( m_texture ) {
		//Log( "Clearing texture" );
		m_texture = nullptr;
		if ( m_background ) {
			m_background->SetTexture( nullptr );
		}
	}
}

void Surface::SetStretchTexture( const bool stretch_texture ) {
	ASSERT( m_texture, "stretch texture but texture not set" );
	if ( stretch_texture != m_stretch_texture ) {
		m_stretch_texture = stretch_texture;
		Realign();
	}
}

void Surface::Create() {
	UIObject::Create();

	NEW( m_background_mesh, scene::mesh::Rectangle );
	NEW( m_background, scene::actor::MeshActor, "UI::Surface", m_background_mesh );
	AddActor( m_background );
}

void Surface::Destroy() {

	RemoveActor( m_background );
	DELETE( m_background );

	UIObject::Destroy();
}

void Surface::Align() {
	UIObject::Align();

	const Vec2<coord_t> v1 = { ClampX( m_object_area.left ), ClampY( m_object_area.top ) };
	const Vec2<coord_t> v2 = { ClampX( m_object_area.right ), ClampY( m_object_area.bottom ) };
	
	if ( m_texture && !m_stretch_texture ) {
		m_background_mesh->SetCoords( v1, v2, { m_texture->m_width, m_texture->m_height }, -m_z_index );
	}
	else {
		m_background_mesh->SetCoords( v1, v2, -m_z_index );
	}
	//m_background->Update();
}

void Surface::Draw() {
	UIObject::Draw();

	if ( m_texture ) {
		m_background->SetTexture( m_texture );
	}
}

void Surface::ApplyStyle() {
	UIObject::ApplyStyle();
	
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

}
}
