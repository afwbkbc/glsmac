#pragma once

#include <map>

#include "types/Serializable.h"

#include "Faction.h"
#include "DifficultyLevel.h"

namespace game {
namespace rules {

CLASS( Rules, types::Serializable )

	std::map< size_t, Faction > m_factions;
	std::map< size_t, DifficultyLevel > m_difficulty_levels;

	virtual const Faction& GetDefaultFaction() const = 0;
	virtual const DifficultyLevel& GetDefaultDifficultyLevel() const = 0;

	void Initialize();

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

protected:
	virtual void InitRules() = 0;

private:
	bool m_is_initialized = false;

};

}
}
