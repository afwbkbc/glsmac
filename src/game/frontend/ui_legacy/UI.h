#pragma once

#include "ui_legacy/object/UIContainer.h"

namespace game {
namespace frontend {

class Game;

namespace ui_legacy {

CLASS( UI, ::ui_legacy::object::UIContainer )

	UI( Game* game, const std::string& class_name );

protected:
	Game* m_game = nullptr;

};

}
}
}
