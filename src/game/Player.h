#pragma once

#include <string>

#include "types/Serializable.h"

#include "game/rules/Faction.h"

namespace game {

CLASS( Player, types::Serializable )
	
	enum role_t {
		PR_NONE,
		PR_HOST,
		PR_PLAYER,
	};
	
	Player();
	Player( const std::string& name, const role_t role, const rules::Faction& faction );

	const bool IsInitialized() const;
	const std::string& GetName() const;
	
	void SetFaction( const rules::Faction& faction );
	const rules::Faction& GetFaction() const;
	
	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );
	
private:
	bool m_is_initialized = false;
	std::string m_name = "";
	role_t m_role = PR_NONE;
	
	rules::Faction m_faction = {};
	
};

}
