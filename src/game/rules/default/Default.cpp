#include "Default.h"

using namespace types;

namespace game {
namespace rules {

void Default::InitRules() {

	m_difficulty_levels = {
		{ DT_CITIZEN,    { "Citizen",    -3 } },
		{ DT_SPECIALIST, { "Specialist", -2 } },
		{ DT_TALENT,     { "Talent",     -1 } },
		{ DT_LIBRARIAN,  { "Librarian",  0 } },
		{ DT_THINKER,    { "Thinker",    1 } },
		{ DT_TRANSCEND,  { "Transcend",  2 } },
	};

}

const DifficultyLevel& Default::GetDefaultDifficultyLevel() const {
	return m_difficulty_levels.at( DT_TRANSCEND );
}

}
}
