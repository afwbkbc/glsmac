#pragma once

#include <string>

#include "types/Serializable.h"
#include "gse/Wrappable.h"

namespace game {
namespace backend {

namespace faction {
class Faction;
}

namespace slot {
class Slot;
}

CLASS2( Player, types::Serializable, gse::Wrappable )

	enum role_t {
		PR_NONE,
		PR_SINGLE,
		PR_HOST,
		PR_PLAYER,
	};

	Player( types::Buffer buf );
	Player(
		const std::string& name,
		const role_t role,
		faction::Faction* faction,
		const std::string& difficulty_level
	);

	const std::string& GetPlayerName() const;
	const std::string GetFullName() const;

	void Connect();
	void Disconnect();
	const bool IsConnected() const;

	void SetFaction( faction::Faction* faction );
	void ClearFaction();
	faction::Faction* GetFaction();

	void SetDifficultyLevel( const std::string& difficulty_level );
	const std::string& GetDifficultyLevel() const;

	void SetSlot( slot::Slot* slot );
	slot::Slot* GetSlot() const;

	const role_t GetRole() const;

	const bool IsTurnCompleted() const;
	void CompleteTurn();
	void UncompleteTurn();

	WRAPDEFS_PTR( Player );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

private:

	bool m_is_connected = false;

	std::string m_name = "";
	role_t m_role = PR_NONE;

	slot::Slot* m_slot = nullptr;
	size_t m_slotnum = 0;

	faction::Faction* m_faction = {};
	std::string m_difficulty_level = "";

	bool m_is_turn_completed = false;
};

}
}
