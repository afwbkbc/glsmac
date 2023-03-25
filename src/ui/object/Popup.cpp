#include "Popup.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

Popup::Popup( const std::string& class_name )
	: UIContainer( class_name )
{
	SetAlign( ALIGN_CENTER );

}

void Popup::Create() {
	UIContainer::Create();
	
	// TODO: fix blending
	// TODO: maybe not needed?
	/* NEW( m_background_locker, Surface, "PopupBackground" );
	g_engine->GetUI()->AddObject( m_background_locker ); */
}

void Popup::Destroy() {
	// TODO: fix blending
	// TODO: maybe not needed?
	//g_engine->GetUI()->RemoveObject( m_background_locker );
	
	UIContainer::Destroy();
}

void Popup::Open() {
	ASSERT( !m_is_opened, "popup already opened" );
	m_is_opened = true;
	g_engine->GetUI()->OpenPopup( this );
}

void Popup::Close() {
	ASSERT( m_is_opened, "popup already closed" );
	g_engine->GetUI()->ClosePopup( this );
}

bool Popup::MaybeClose() {
	ASSERT( m_is_opened, "maybeclose but popup not opened" );
	m_is_opened = false;
	return true;
}

}
}
