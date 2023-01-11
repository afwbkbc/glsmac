#include "MenuItem.h"

using namespace std;
using namespace ui::object;

namespace task {
namespace mainmenu {

MenuItem::MenuItem( const Menu* menu, const string& text )
	: UIContainer()
	, m_menu( menu )
	, m_text( text )
{
	
	SetHeight( 68 );
	SetLeft( 0 );
	SetRight( 0 );
	
	SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
	
}

void MenuItem::Create() {
	UIContainer::Create();

	const auto* textures = &(m_menu->m_task->m_menu_item_textures);
	
	m_body = new Surface;
	m_body->SetTop(14);
	m_body->SetLeft(11);
	m_body->SetHeight(32);
	m_body->SetWidth(284);
	m_body->SetTexture( (*textures)[5] );
	AddChild(m_body);
	
	m_left = new Surface;
	m_left->SetTop(14);
	m_left->SetLeft(6);
	m_left->SetTexture( (*textures)[3] );
	m_left->SetSizeFromTexture();
	AddChild(m_left);
	
	m_right = new Surface;
	m_right->SetTop(14);
	m_right->SetLeft(294);
	m_right->SetTexture( (*textures)[4] );
	m_right->SetSizeFromTexture();
	AddChild(m_right);
	
	m_lower_top = new Surface;
	m_lower_top->SetTop(6);
	m_lower_top->SetLeft(0);
	m_lower_top->SetTexture( (*textures)[1] );
	m_lower_top->SetSizeFromTexture();
	AddChild(m_lower_top);
	
	m_upper_top = new Surface;
	m_upper_top->SetTop(0);
	m_upper_top->SetLeft(200);
	m_upper_top->SetTexture( (*textures)[0] );
	m_upper_top->SetSizeFromTexture();
	AddChild(m_upper_top);
	
	m_bottom = new Surface;
	m_bottom->SetTop(44);
	m_bottom->SetLeft(6);
	m_bottom->SetTexture( (*textures)[2] );
	m_bottom->SetSizeFromTexture();
	AddChild(m_bottom);
	
	// normal types::Color::FromRGBA(94, 162 ,137, 255)
	// highlight types::Color::FromRGBA(193, 240 ,204, 255)
	
	types::Color c;
	if (m_text == "START GAME") {
		c = types::Color::FromRGBA(193, 240 ,204, 255);
	}
	else {
		c = types::Color::FromRGBA(94, 162 ,137, 255);
	}
	
	m_label = new Label( m_menu->m_task->m_menu_item_font, m_text, c );
	m_label->SetLeft( -56 );
	m_label->SetTop( 20 );
	AddChild(m_label);
}

void MenuItem::Destroy() {
	RemoveChild(m_upper_top);
	RemoveChild(m_lower_top);
	RemoveChild(m_bottom);
	RemoveChild(m_left);
	RemoveChild(m_right);
	RemoveChild(m_body);
	RemoveChild(m_label);
	
	UIContainer::Destroy();
}

}
}
