#include "MainMenuBackground.h"

#include "ui/object/UIObject.h"

using namespace ui;
namespace ui {
using namespace object;
}

namespace game {
namespace mainmenu {

void MainMenuBackground::SetStyle() {
	Set( A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	SetTexture( A_TEXTURE, "xopeningb.pcx" );
	Set( A_STRETCH_TEXTURE );
}

}
}
