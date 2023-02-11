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

}
}
