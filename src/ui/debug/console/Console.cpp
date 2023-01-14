#include "Console.h"
#include "engine/Engine.h"

namespace ui {

using namespace object;

namespace debug {

void Console::Create() {
	UIContainer::Create();
/*
	types::Texture* m_body_background = g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 959, 463, 1014, 518,
		loader::texture::TextureLoader::LT_TILED | 
		loader::texture::TextureLoader::LT_ALPHA
			, 0.5
	);
	*/
	/*NEW( m_body, Surface );
	m_body->SetLeft( 0 );
	m_body->SetRight( 0 );
	m_body->SetTop( 0 );
	m_body->SetHeight( 300 );*/
	/*m_body->SetZIndex( -1 );
	m_body->SetTexture( m_body_background );
	*/
	//SetTexture( m_body_background );
	
	//AddChild( m_body );

	//ShowDebugFrame();
	
}

void Console::UpdateHeight() {
	SetHeight( g_engine->GetRenderer()->GetWindowHeight() / 2 );
}

void Console::Destroy() {
	
	//RemoveChild( m_body );
	
	UIContainer::Destroy();
}

void Console::Iterate() {
	UIContainer::Iterate();
}

}
}