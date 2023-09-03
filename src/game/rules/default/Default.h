#pragma once

#include "../Rules.h"

namespace game {
namespace rules {

CLASS( Default, Rules )

	const Faction& GetDefaultFaction() const;
	const DifficultyLevel& GetDefaultDifficultyLevel() const;

protected:
	void InitRules();

private:

	enum faction_type_t {
		FT_RANDOM,
		FT_GAIANS,
		FT_HIVE,
		FT_UNIVERSITY,
		FT_MORGANITES,
		FT_SPARTANS,
		FT_BELIEVERS,
		FT_PEACEKEEPERS,
		FT_CONSCIOUSNESS,
		FT_PIRATES,
		FT_DRONES,
		FT_ANGELS,
		FT_PLANETCULT,
		FT_CARETAKERS,
		FT_USURPERS,
	};

	enum difficulty_type_t {
		DT_CITIZEN,
		DT_SPECIALIST,
		DT_TALENT,
		DT_LIBRARIAN,
		DT_THINKER,
		DT_TRANSCEND,
	};
};

}
}
