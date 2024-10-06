#pragma once

#include "common/Task.h"

#include "ui/Types.h"

namespace game {
namespace backend {
class State;
}
namespace frontend {
class Game;
}
}

namespace task {
namespace game {

CLASS( Game, common::Task )

	Game( ::game::backend::State* state, ::ui::ui_handler_t on_start = 0, ::ui::ui_handler_t on_cancel = 0 );
	~Game();

	void Start() override;
	void Stop() override;
	void Iterate() override;

private:
	::game::frontend::Game* const m_game = nullptr;

};

}
}
