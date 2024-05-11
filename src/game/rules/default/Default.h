#pragma once

#include "game/rules/Rules.h"

namespace game {
namespace rules {

CLASS( Default, Rules )

	const DifficultyLevel& GetDefaultDifficultyLevel() const override;

protected:
	void InitRules() override;

private:

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
