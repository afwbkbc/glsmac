#pragma once

#include <unordered_map>

#include "gse/Bindings.h"
#include "Player.h"
#include "Turn.h"
#include "rules/Faction.h"
#include "rules/DifficultyLevel.h"

namespace game {
class Game;

class Bindings : public gse::Bindings {
public:
	Bindings( Game* game );
	void AddToContext( gse::Context* ctx ) override;

private:

	enum callback_slot_t {
		CS_ONTURN,
	};
	std::unordered_map< callback_slot_t, const gse::type::Callable* > m_callbacks = {};

	Game* m_game;

	const gse::Value GetPlayer( const Player* player ) const;
	const gse::Value GetFaction( const rules::Faction* faction ) const;
	const gse::Value GetDifficulty( const rules::DifficultyLevel* difficulty ) const;
	const gse::Value GetTurn( const Turn* turn ) const;

	void SetCallback( const callback_slot_t slot, const gse::type::Callable* callback, const gse::Context* context, const gse::si_t& si );
};

}
