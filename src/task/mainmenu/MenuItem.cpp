#include "MenuItem.h"

using namespace std;
using namespace ui::object;
using namespace ui::event;

namespace task {
namespace mainmenu {

MenuItem::MenuItem( const Menu* menu, const string& text )
	: UIContainer()
	, m_menu( menu )
	, m_text( text )
{
	
	SetHeight( 46 );
	SetWidth( 300 );
	SetLeft( 0 );
	
	SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
	
}

void MenuItem::Create() {
	UIContainer::Create();

	const auto* textures = &(m_menu->m_task->m_menu_item_textures);
	
	m_body = new Surface;
	m_body->SetTop(6);
	m_body->SetLeft(11);
	m_body->SetHeight(36);
	m_body->SetWidth(284);
	m_body->SetTexture( (*textures)[5] );
	AddChild(m_body);
	
	m_left = new Surface;
	m_left->SetTop(6);
	m_left->SetLeft(6);
	m_left->SetTexture( (*textures)[3] );
	m_left->SetSizeFromTexture();
	AddChild(m_left);
	
	m_right = new Surface;
	m_right->SetTop(6);
	m_right->SetLeft(294);
	m_right->SetTexture( (*textures)[4] );
	m_right->SetSizeFromTexture();
	AddChild(m_right);
	
	m_lower_top = new Surface;
	m_lower_top->SetTop(-2);
	m_lower_top->SetLeft(0);
	m_lower_top->SetTexture( (*textures)[1] );
	m_lower_top->SetSizeFromTexture();
	AddChild(m_lower_top);
	
	m_upper_top = new Surface;
	m_upper_top->SetTop(-8);
	m_upper_top->SetLeft(200);
	m_upper_top->SetTexture( (*textures)[0] );
	m_upper_top->SetSizeFromTexture();
	AddChild(m_upper_top);
	
	m_bottom = new Surface;
	m_bottom->SetTop(40);
	m_bottom->SetLeft(6);
	m_bottom->SetTexture( (*textures)[2] );
	m_bottom->SetSizeFromTexture();
	AddChild(m_bottom);
	
	m_label = new Label( m_menu->m_task->m_menu_item_font, m_text, m_menu->m_task->m_menu_item_text_color_normal );
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

void MenuItem::OnMouseOver( const UIEvent::event_data_t* data ) {
	m_label->SetTextColor( m_menu->m_task->m_menu_item_text_color_highlight );
}

void MenuItem::OnMouseOut( const UIEvent::event_data_t* data ) {
	m_label->SetTextColor( m_menu->m_task->m_menu_item_text_color_normal );
}


}
}
