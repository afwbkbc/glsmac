#pragma once

#include <map>

#include "types/Serializable.h"

#include "Faction.h"
#include "DifficultyLevel.h"

namespace game {
namespace rules {

CLASS( Rules, types::Serializable )

	typedef std::unordered_map< std::string, Faction > factions_t;

	factions_t m_factions;
	std::map< size_t, DifficultyLevel > m_difficulty_levels;

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
