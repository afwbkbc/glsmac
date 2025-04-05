#include "Default.h"

namespace game {
namespace backend {
namespace rules {

void Default::InitRules() {
}

static const std::string s_default_difficulty_level = "Transcend"; // we're not casuals
const std::string& Default::GetDefaultDifficultyLevel() const {
	return s_default_difficulty_level;
}

}
}
}
