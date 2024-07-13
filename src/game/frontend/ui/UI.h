#pragma once

#include "ui/object/UIContainer.h"

namespace game {
namespace frontend {

class Game;

namespace ui {

CLASS( UI, ::ui::object::UIContainer )

	UI( Game* game, const std::string& class_name );

protected:
	Game* m_game = nullptr;

};

}
}
}
