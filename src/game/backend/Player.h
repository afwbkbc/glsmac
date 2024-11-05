#pragma once

#include <string>

#include "types/Serializable.h"

#include "Faction.h"
#include "game/backend/rules/DifficultyLevel.h"

namespace game {
namespace backend {

namespace slot {
class Slot;
}

CLASS( Player, types::Serializable )

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
		Faction* faction,
		const rules::DifficultyLevel& difficulty_level
	);

	const std::string& GetPlayerName() const;
	const std::string GetFullName() const;

	void Connect();
	void Disconnect();
	const bool IsConnected() const;

	void SetFaction( Faction* faction );
	void ClearFaction();
	Faction* GetFaction();

	void SetDifficultyLevel( const rules::DifficultyLevel& difficulty_level );
	const rules::DifficultyLevel& GetDifficultyLevel() const;

	void SetSlot( slot::Slot* slot );
	slot::Slot* GetSlot() const;

	const role_t GetRole() const;

	const bool IsTurnCompleted() const;
	void CompleteTurn();
	void UncompleteTurn();

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

private:

	bool m_is_connected = false;

	std::string m_name = "";
	role_t m_role = PR_NONE;

	slot::Slot* m_slot = nullptr;

	Faction* m_faction = {};
	rules::DifficultyLevel m_difficulty_level = {};

	bool m_is_turn_completed = false;
};

}
}
