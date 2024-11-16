#pragma once

#include "util/random/Random.h"
#include "gse/Wrappable.h"

#include "gse/type/Object.h"

namespace game {
namespace backend {

class Game;

CLASS2( Random, util::random::Random, gse::Wrappable )

	Random( Game* game, const util::random::value_t seed = 0 );

	WRAPDEFS_PTR( Random )

private:
	Game* m_game = nullptr;
};

}
}
