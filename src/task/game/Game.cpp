#include "Game.h"

#include "game/frontend/Game.h"

namespace task {
namespace game {

Game::Game( ::game::backend::State* state, ::ui_legacy::ui_handler_t on_start, ::ui_legacy::ui_handler_t on_cancel )
	: m_game( new ::game::frontend::Game( this, nullptr, state, nullptr, on_start, on_cancel ) ) {

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
