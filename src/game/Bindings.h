#pragma once

#include "gse/Bindings.h"
#include "Player.h"
#include "rules/Faction.h"
#include "rules/DifficultyLevel.h"

namespace game {
class Game;

class Bindings : public gse::Bindings {
public:
	Bindings( Game* game );
	void AddToContext( gse::Context* ctx ) const override;
private:
	Game* m_game;

	const gse::Value GetPlayer( const Player* player ) const;
	const gse::Value GetFaction( const rules::Faction* faction ) const;
	const gse::Value GetDifficulty( const rules::DifficultyLevel* difficulty ) const;
};

}
