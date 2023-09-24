#pragma once

#include "ui/object/UIContainer.h"

namespace task {
namespace game {
class Game;
namespace ui {

CLASS( UI, UIContainer )

	UI( Game* game, const std::string& class_name );

protected:
	Game* m_game = nullptr;

};

}
}
}
