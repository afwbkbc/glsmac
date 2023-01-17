#include "MainMenuBackground.h"

#include "ui/object/UIObject.h"

#include "engine/Engine.h"

using namespace ui;
namespace ui {
using namespace object;
}

namespace game {
namespace mainmenu {

void MainMenuBackground::SetStyle() {
	Set( A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	SetObject( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture( "xopeningb.pcx" ) );
	Set( A_STRETCH_TEXTURE );
}

}
}
