#pragma once

#include <string>

#include "types/Serializable.h"

#include "game/rules/Rules.h"

namespace game {

class Slot;

CLASS( Player, types::Serializable )

	const static rules::Faction RANDOM_FACTION;

	enum role_t {
		PR_NONE,
		PR_SINGLE,
		PR_HOST,
		PR_PLAYER,
	};

	Player( const rules::Rules& rules, types::Buffer buf );
	Player(
		const rules::Rules& rules,
		const std::string& name,
		const role_t role,
		const rules::Faction& faction,
		const rules::DifficultyLevel& difficulty_level
	);

	const bool IsInitialized() const;
	const std::string& GetPlayerName() const;
	const std::string GetFullName() const;

	void Connect();
	void Disconnect();
	const bool IsConnected() const;

	void SetFaction( const rules::Faction& faction );
	const rules::Faction& GetFaction() const;

	void SetDifficultyLevel( const rules::DifficultyLevel& difficulty_level );
	const rules::DifficultyLevel& GetDifficultyLevel() const;

	void SetSlot( Slot* slot );
	Slot* GetSlot() const;

	const role_t GetRole() const;

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

private:

	const rules::Rules& m_rules;

	bool m_is_initialized = false;
	bool m_is_connected = false;

	std::string m_name = "";
	role_t m_role = PR_NONE;

	Slot* m_slot = nullptr;

	rules::Faction m_faction = RANDOM_FACTION;
	rules::DifficultyLevel m_difficulty_level = {};

};

}

#include "game/Slot.h"
