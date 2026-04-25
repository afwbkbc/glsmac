#pragma once

#include <unordered_map>
#include <vector>

#include "types/Serializable.h"
#include "gse/Wrappable.h"

namespace game {
namespace backend {
namespace rules {

CLASS2( Rules, types::Serializable, gse::Wrappable )

	wrapmap_t< int > m_difficulty_levels{ {
		{ -3, "Citizen", },
		{ -2, "Specialist", },
		{ -1, "Talent", },
		{ 0, "Librarian", },
		{ 1, "Thinker", },
		{ 2, "Transcend", },
	} }; // TODO: fix and wrap DifficultyLevel class, for now just hardcode and map name to difficulty

	virtual const std::string& GetDefaultDifficultyLevel() const = 0;
	const int GetDefaultDifficultyLevelV() const;

	void Initialize();

	WRAPDEFS_DYNAMIC( Rules );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;

protected:
	virtual void InitRules() = 0;

private:
	bool m_is_initialized = false;

	const gse::value::array_elements_t WrapDifficultyLevels( gc::Space* const gc_space );

};

}
}
}
