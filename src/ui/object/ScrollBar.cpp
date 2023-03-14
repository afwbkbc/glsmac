#include "ScrollBar.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

ScrollBar::ScrollBar()
	: UIContainer()
{
	// TODO: styles?
	
	SetAlign( ALIGN_RIGHT | ALIGN_VCENTER );
	SetWidth( 11 );
	SetTop( 0 );
	SetBottom( 0 );
	//ShowDebugFrame();
	
}

void ScrollBar::Create() {
	UIContainer::Create();
	
	auto* tl = g_engine->GetTextureLoader();
	
	auto* ui = g_engine->GetUI();
	
	NEW( m_background, Surface );
		m_background->SetTexture( tl->GetColorTexture( Color( 0.5f, 0.5f, 0.5f, 1.0f ) ) );
		//m_background->SetTintColor( Color( 0.5f, 0.5f, 0.5f, 1.0f ) );
		//m_background->SetTexture( tl->LoadTexture( "interface.pcx", 86, 665, 109, 688 ) );
		
	AddChild( m_background );
}

void ScrollBar::Destroy() {
	
	auto* ui = g_engine->GetUI();
	
	RemoveChild( m_background );
	
	UIContainer::Destroy();
}

}
}
