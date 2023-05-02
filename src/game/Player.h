#pragma once

#include <string>

#include "types/Serializable.h"

namespace game {

CLASS( Player, types::Serializable )
	
	enum role_t {
		PR_NONE,
		PR_HOST,
		PR_PLAYER,
	};
	
	Player();
	Player( const std::string& name, const role_t role );

	const bool IsInitialized() const;
	const std::string& GetName() const;
	
	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );
	
private:
	bool m_is_initialized = false;
	std::string m_name = "";
	role_t m_role = PR_NONE;
	
};

}
