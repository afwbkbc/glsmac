#include "IntroLogo.h"

#include "ui/object/UIObject.h"

#include "engine/Engine.h"

namespace game {
namespace intro {

void IntroLogo::SetStyle() {
	Set( A_ALIGN, ui::object::UIObject::ALIGN_CENTER );
	SetObject( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture( "logo.pcx" ) );
	Set( A_STRETCH_TEXTURE );
}

}
}
