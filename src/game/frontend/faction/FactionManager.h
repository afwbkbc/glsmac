#pragma once

#include <unordered_map>

#include "common/Common.h"

namespace game {

namespace backend {
class Faction;
}

namespace frontend {

class Game;

namespace faction {

class Faction;

CLASS( FactionManager, common::Class )

	FactionManager( Game* game );

	void DefineFaction( const backend::Faction* def );

	Faction* GetFactionById( const std::string& id ) const;

private:

	Game* m_game = nullptr;

	std::unordered_map< std::string, Faction* > m_factions = {};

};

}
}
}
