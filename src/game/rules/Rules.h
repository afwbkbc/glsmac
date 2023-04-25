#pragma once

#include <map>

#include "types/Serializable.h"

#include "Faction.h"

namespace game {
namespace rules {

CLASS( Rules, types::Serializable )

	std::map< size_t, Faction > m_factions;

	void Initialize();

	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );
	
protected:
	virtual void InitRules() = 0;
	
private:
	bool m_is_initialized = false;
	
};

}
}
