#include "MenuItem.h"

using namespace ui::object;

namespace task {
namespace mainmenu {

MenuItem::MenuItem( const MenuItemMesh* mesh )
	: Button()
	, m_mesh( mesh )
{
	
	SetHeight( 100 );
	SetWidth( 300 );
	SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
	
	SetBackgroundColor(types::Color::TRANSPARENT());
	
	
	
}

void MenuItem::Create() {
	Button::Create();
	
	m_actor = new scene::actor::MeshActor( "MainMenu::MenuItem", m_mesh );
	
	m_actors.push_back( m_actor );
	
	/*m_background_mesh = new scene::mesh::vec2::Rectangle();
	m_background = new scene::actor::MeshActor( "UI::Surface", m_background_mesh );
	m_actors.push_back( m_background );*/
	
}

void MenuItem::Destroy() {

	//delete m_background;

	Button::Destroy();
}

/*void Align() {
	UIObject::Align();

	m_background_mesh->SetCoords(
		ClampX( m_object_area.left ),
		ClampY( m_object_area.top ),
		ClampX( m_object_area.right ),
		ClampY( m_object_area.bottom )
	);
}*/
void MenuItem::Draw() {
	Button::Draw();
/*
	m_background->SetTintColor( m_colors.background );
	if ( m_texture ) {
		m_background->SetTexture( m_texture );
	}*/
}


}
}
