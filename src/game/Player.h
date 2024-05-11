#pragma once

#include <string>
#include <optional>

#include "types/Serializable.h"

#include "rules/Faction.h"
#include "rules/DifficultyLevel.h"

namespace game {

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
		const rules::Faction& faction,
		const rules::DifficultyLevel& difficulty_level
	);

	const std::string& GetPlayerName() const;
	const std::string GetFullName() const;

	void Connect();
	void Disconnect();
	const bool IsConnected() const;

	void SetFaction( const rules::Faction& faction );
	void ClearFaction();
	std::optional< rules::Faction >& GetFaction();

	void SetDifficultyLevel( const rules::DifficultyLevel& difficulty_level );
	const std::optional< rules::DifficultyLevel >& GetDifficultyLevel() const;

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

	// EXPERIMENTAL
	std::optional< rules::Faction > m_faction = {};
	std::optional< rules::DifficultyLevel > m_difficulty_level = {};

	bool m_is_turn_completed = false;
};

}
