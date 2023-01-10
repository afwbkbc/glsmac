#include "MenuItem.h"

using namespace std;
using namespace ui::object;

namespace task {
namespace mainmenu {

MenuItem::MenuItem( const vector<const types::Texture*>* textures )
	: UIContainer()
	, m_textures( textures )
	//, m_mesh( mesh )
{
	
	//SetWidth( background->m_width );
	//SetHeight( background->m_height );
	
	SetWidth( 400 );
	SetHeight( 100 );
	
	/*m_panel = new Panel;
	
	m_panel->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_panel->SetWidth(400);
	m_panel->SetHeight(600);
	
	m_panel->SetBackgroundColor(types::Color::TRANSPARENT());
	
	m_panel->AddChild(test);*/
	
	
	
	SetAlign( UIObject::ALIGN_CENTER );
	
	//SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
	
	//SetBackgroundTexture( background );
	
	//SetBackgroundColor(types::Color::TRANSPARENT());
	
	/*this->SetBackgroundTexture( (*m_textures)[4] );
	this->SetBorderWidth( 10 );*/
	
}

void MenuItem::Create() {
	UIContainer::Create();

	m_top = new Surface;
	m_top->SetTop(0);
	m_top->SetLeft(0);
	m_top->SetTexture( (*m_textures)[0] );
	m_top->SetSizeFromTexture();
	AddChild(m_top);
	
	m_left = new Surface;
	m_left->SetTop(8);
	m_left->SetLeft(6);
	m_left->SetTexture( (*m_textures)[2] );
	m_left->SetSizeFromTexture();
	AddChild(m_left);
	
	m_right = new Surface;
	m_right->SetTop(8);
	m_right->SetLeft(240);
	m_right->SetTexture( (*m_textures)[3] );
	m_right->SetSizeFromTexture();
	AddChild(m_right);
	
	m_bottom = new Surface;
	m_bottom->SetTop(46);
	m_bottom->SetLeft(6);
	m_bottom->SetTexture( (*m_textures)[1] );
	m_bottom->SetSizeFromTexture();
	AddChild(m_bottom);
	
	m_body = new Surface;
	m_body->SetTop(8);
	m_body->SetLeft(11);
	m_body->SetHeight(38);
	m_body->SetWidth(230);
	m_body->SetTexture( (*m_textures)[4] );
	AddChild(m_body);
	
	

	/*
	m_actor = new scene::actor::MeshActor( "MainMenu::MenuItem", m_mesh );
	m_actor->SetTintColor({ 0.5, 0.7, 0.8, 0.5});
	m_actors.push_back( m_actor );
	*/
	/*m_background_mesh = new scene::mesh::vec2::Rectangle();
	m_background = new scene::actor::MeshActor( "UI::Surface", m_background_mesh );
	m_actors.push_back( m_background );*/
	
	//this->SetBackgroundTexture( (*m_textures)[4] );
	//this-
	/*this->m_borders.top->SetTexture( (*m_textures)[0] );
	this->m_borders.bottom->SetTexture( (*m_textures)[1] );*/
}

void MenuItem::Destroy() {

	//delete m_background;

	RemoveChild(m_top);
	RemoveChild(m_bottom);
	RemoveChild(m_left);
	RemoveChild(m_right);
	RemoveChild(m_body);
	
	UIContainer::Destroy();
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
	UIContainer::Draw();
/*
	m_background->SetTintColor( m_colors.background );
	if ( m_texture ) {
		m_background->SetTexture( m_texture );
	}*/
}


}
}
