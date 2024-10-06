#include "Game.h"

#include "game/frontend/Game.h"

namespace task {
namespace game {

Game::Game( ::game::backend::State* state, ::ui::ui_handler_t on_start, ::ui::ui_handler_t on_cancel )
	: m_game( new ::game::frontend::Game( this, state, on_start, on_cancel ) ) {

}

Game::~Game() {
	delete m_game;
}

void Game::Start() {
	m_game->Start();
}

void Game::Stop() {
	m_game->Stop();
}

void Game::Iterate() {
	m_game->Iterate();
}

}
}
