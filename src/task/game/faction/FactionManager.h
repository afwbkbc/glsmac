#pragma once

#include <unordered_map>

#include "common/Common.h"

namespace game::rules {
class Faction;
}

namespace task {
namespace game {

class Game;

namespace faction {

class Faction;

CLASS( FactionManager, common::Class )

	FactionManager( Game* game );

	void DefineFaction( const ::game::rules::Faction* def );

	Faction* GetFactionById( const std::string& id ) const;

private:

	Game* m_game = nullptr;

	std::unordered_map< std::string, Faction* > m_factions = {};

};

}
}
}
