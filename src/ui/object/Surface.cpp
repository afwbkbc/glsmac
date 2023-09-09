#include "Surface.h"

using namespace types;

namespace ui {
namespace object {

Surface::Surface( const std::string& class_name )
	: Mesh( class_name )
{
	//
}

void Surface::SetTexture( types::Texture* texture ) {
	if ( texture != m_texture ) {
		Mesh::SetTexture( texture );
		Resize();
	}
}

void Surface::SetStretchTexture( const bool stretch_texture ) {
	ASSERT( m_texture, "stretch texture but texture not set" );
	if ( stretch_texture != m_stretch_texture ) {
		m_stretch_texture = stretch_texture;
		m_force_resize = true; // because Mesh doesn't know about m_stretch_texture
		Resize();
	}
}

void Surface::Create() {
	Mesh::Create();

	NEW( m_background_mesh, types::mesh::Rectangle );
	ResizeNow();
	SetMesh( m_background_mesh );
}

void Surface::Iterate() {
	if ( m_need_resize ) {
		ResizeNow();
	}
	
	Mesh::Iterate();
}

void Surface::ApplyStyle() {
	Mesh::ApplyStyle();

	if ( Has( Style::A_TEXTURE ) ) {
		auto* texture = (Texture*)GetObject( Style::A_TEXTURE );
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

	//if ( m_need_resize ) { // TODO: fix
		ResizeNow();
	//}
	
	Mesh::Align();
}

void Surface::Resize() {
	m_need_resize = true;
}

void Surface::ResizeNow() {
	
	if ( m_background_mesh ) {
		
		bool need_resize = false;
		
		if ( // TODO: use ints for object area?
			round( m_surface_object_area.width ) != round( m_object_area.width ) ||
			round( m_surface_object_area.height ) != round( m_object_area.height )
		) {
			//Log( "Resizing because object area has changed ( " + std::to_string( m_last_object_area.width ) + " != " + std::to_string( m_object_area.width ) + " || " + std::to_string( m_last_object_area.height ) + " != " + std::to_string( m_object_area.height ) + " )" );
			m_surface_object_area = m_object_area;
			need_resize = true;
		}
		
		if ( m_texture && (
			m_last_texture_size.x != m_texture->m_width ||
			m_last_texture_size.y != m_texture->m_height
		)) {
			//Log( "Resizing because texture size has changed" );
			m_surface_texture_size = { m_texture->m_width, m_texture->m_height };
			need_resize = true;
		}
		
		if ( m_surface_stretch_texture != m_stretch_texture ) {
			//Log( "Resizing because texture stretch has changed" );
			m_surface_stretch_texture = m_stretch_texture;
			need_resize = true;
		}

		if ( need_resize ) {
			Vec2< coord_t > pos = { 0.0f, 0.0f };
			Vec2< coord_t > size = {
				ClampX( m_surface_object_area.right ) - ClampX( m_surface_object_area.left ),
				ClampY( m_surface_object_area.bottom ) - ClampY( m_surface_object_area.top )
			};

			//Log( "Resizing surface to " + size.ToString() );

			if ( m_texture && !m_surface_stretch_texture ) {
				m_background_mesh->SetCoords( pos, size, { m_texture->m_width, m_texture->m_height }, -m_z_index );
			}
			else {
				m_background_mesh->SetCoords( pos, size, -m_z_index );
			}
		}
		
		m_need_resize = false;
	}
}

}
}
