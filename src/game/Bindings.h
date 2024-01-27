#pragma once

#include "gse/Bindings.h"

namespace game {
class Game;

class Bindings : public gse::Bindings {
public:
	Bindings( Game* game );
	void AddToContext( gse::Context* ctx ) const override;
private:
	Game* m_game;
};

}
