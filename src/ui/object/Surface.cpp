#include "Surface.h"

namespace ui {
namespace object {

Surface::Surface( const string& class_name )
	: UIObject( class_name )
{
	//
}

void Surface::SetTexture( const types::Texture* texture) {
	m_texture = texture;
}

void Surface::Create() {
	UIObject::Create();

	m_background_mesh = new scene::mesh::Rectangle();
	m_background = new scene::actor::MeshActor( "UI::Surface", m_background_mesh );
	m_actors.push_back( m_background );
}

void Surface::Destroy() {

	delete m_background;

	UIObject::Destroy();
}

void Surface::Align() {
	UIObject::Align();

	m_background_mesh->SetCoords(
		{ ClampX( m_object_area.left ), ClampY( m_object_area.top ) },
		{ ClampX( m_object_area.right ), ClampY( m_object_area.bottom ) }
	);
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
		auto* texture = GetTexture( Style::A_TEXTURE );
		SetTexture( texture );
		if ( Has( Style::A_SIZE_FROM_TEXTURE ) ) {
			SetWidth( texture->m_width );
			SetHeight( texture->m_height );
		}
	}
	
}

}
}
