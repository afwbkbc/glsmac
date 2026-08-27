#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>

#include "types/Serializable.h"
#include "gse/Wrappable.h"

namespace game {
namespace backend {

class Game;

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
		PR_AI,
		PR_NATIVE,
	};

	Player( types::Buffer buf );
	Player(
		const std::string& name,
		const role_t role,
		faction::Faction* faction,
		const std::string& difficulty_level
	);
	Player( const Player* const other );
	~Player() override;

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
	const bool IsAI() const;
	const bool IsNative() const;

	const bool IsTurnCompleted() const;
	void CompleteTurn();
	void UncompleteTurn();

	using technologies_t = std::set< std::string >;
	static constexpr int64_t MAX_RESEARCH_PROGRESS = 1000000;
	static constexpr size_t MAX_TECHNOLOGIES = 1024;
	static constexpr int64_t MAX_ENERGY_CREDITS = 1000000000;
	static constexpr int64_t MAX_ECOLOGICAL_DAMAGE_EVENTS = 1000000;
	static constexpr int64_t MAX_MAJOR_ATROCITIES = 1000000;
	static constexpr int64_t MAX_SANCTION_TURNS = 1000000;
	static constexpr int64_t MAX_INTEGRITY_BLEMISHES = 7;
	static constexpr size_t MAX_PROTOTYPED_COMPONENTS = 1024;
	static constexpr size_t MAX_PROTOTYPED_COMPONENT_ID_LENGTH = 128;
	static constexpr size_t MAX_ORBITAL_FACILITY_TYPES = 64;
	static constexpr size_t MAX_ORBITAL_FACILITY_ID_LENGTH = 128;
	static constexpr int64_t MAX_ORBITAL_FACILITY_COUNT = 1000000;

	const technologies_t& GetTechnologies() const;
	bool HasTechnology( const std::string& id ) const;
	const std::string& GetResearchTarget() const;
	int64_t GetResearchProgress() const;
	void SetResearchState(
		const technologies_t& technologies,
		const std::string& target,
		const int64_t progress
	);
	int64_t GetEnergyCredits() const;
	void SetEnergyCredits( const int64_t energy_credits );
	int64_t GetEcologicalDamageEvents() const;
	void SetEcologicalDamageEvents( const int64_t ecological_damage_events );
	int64_t GetMajorAtrocities() const;
	void SetMajorAtrocities( const int64_t major_atrocities );
	int64_t GetSanctionTurns() const;
	void SetSanctionTurns( const int64_t sanction_turns );
	int64_t GetIntegrityBlemishes() const;
	void SetIntegrityBlemishes( const int64_t integrity_blemishes );
	using prototyped_components_t = std::set< std::string >;
	const prototyped_components_t& GetPrototypedComponents() const;
	bool HasPrototypedComponent( const std::string& id ) const;
	void SetPrototypedComponents( const prototyped_components_t& components );
	using orbital_facilities_t = std::map< std::string, int64_t >;
	const orbital_facilities_t& GetOrbitalFacilities() const;
	int64_t GetOrbitalFacilityCount( const std::string& id ) const;
	void SetOrbitalFacilityCount( const std::string& id, const int64_t count );
	int64_t GetOrbitalDefenseDeployments() const;
	void SetOrbitalDefenseDeployments( const int64_t deployments );

	static constexpr int64_t COUNCIL_VOTE_PENDING = -2;
	static constexpr int64_t COUNCIL_VOTE_ABSTAIN = -1;
	static constexpr int64_t COUNCIL_VOTE_NO = 0;
	static constexpr int64_t COUNCIL_VOTE_YES = 1;
	static constexpr int64_t MAX_COUNCIL_TURN = 1000000;
	static constexpr size_t MAX_COUNCIL_PLAYER_ID = 64;
	struct council_state_t {
		bool is_governor = false;
		int64_t last_session_turn = 0;
		std::string proposal = "";
		int64_t caller_id = -1;
		int64_t candidate_a_id = -1;
		int64_t candidate_b_id = -1;
		int64_t vote_id = COUNCIL_VOTE_PENDING;
		bool global_trade_pact = false;
		bool unity_core_salvaged = false;
		bool un_charter_repealed = false;

		bool operator==( const council_state_t& other ) const {
			return
				is_governor == other.is_governor &&
				last_session_turn == other.last_session_turn &&
				proposal == other.proposal &&
				caller_id == other.caller_id &&
				candidate_a_id == other.candidate_a_id &&
				candidate_b_id == other.candidate_b_id &&
				vote_id == other.vote_id &&
				global_trade_pact == other.global_trade_pact &&
				unity_core_salvaged == other.unity_core_salvaged &&
				un_charter_repealed == other.un_charter_repealed;
		}
	};
	const council_state_t& GetCouncilState() const;
	void SetCouncilState( const council_state_t& state );

	using social_engineering_t = std::array< std::string, 4 >;
	static constexpr size_t SOCIAL_ENGINEERING_CATEGORY_COUNT = 4;
	static constexpr size_t MAX_SOCIAL_ENGINEERING_ID_LENGTH = 64;
	const social_engineering_t& GetSocialEngineering() const;
	void SetSocialEngineering( const social_engineering_t& social_engineering );

	enum diplomatic_relation_t {
		DR_NEUTRAL,
		DR_TREATY,
		DR_PACT,
		DR_VENDETTA,
	};
	using diplomatic_relations_t = std::map< size_t, diplomatic_relation_t >;
	static constexpr size_t MAX_DIPLOMATIC_RELATIONS = 64;
	const diplomatic_relations_t& GetDiplomaticRelations() const;
	diplomatic_relation_t GetDiplomaticRelation( const size_t player_id ) const;
	void SetDiplomaticRelation( const size_t player_id, const diplomatic_relation_t relation );
	const diplomatic_relations_t& GetDiplomaticOffers() const;
	diplomatic_relation_t GetDiplomaticOffer( const size_t player_id ) const;
	void SetDiplomaticOffer( const size_t player_id, const diplomatic_relation_t relation );
	static const std::string GetDiplomaticRelationName( const diplomatic_relation_t relation );
	static bool ParseDiplomaticRelation( const std::string& name, diplomatic_relation_t& relation );

	struct diplomatic_trade_t {
		int64_t offer_energy = 0;
		std::string offer_technology = "";
		int64_t request_energy = 0;
		std::string request_technology = "";

		bool operator==( const diplomatic_trade_t& other ) const {
			return
				offer_energy == other.offer_energy &&
				offer_technology == other.offer_technology &&
				request_energy == other.request_energy &&
				request_technology == other.request_technology;
		}
	};
	using diplomatic_trades_t = std::map< size_t, diplomatic_trade_t >;
	static constexpr size_t MAX_DIPLOMATIC_TRADES = 64;
	static constexpr size_t MAX_DIPLOMATIC_TRADE_TECHNOLOGY_ID_LENGTH = 128;
	const diplomatic_trades_t& GetDiplomaticTrades() const;
	const diplomatic_trade_t* GetDiplomaticTrade( const size_t player_id ) const;
	void SetDiplomaticTrade( const size_t player_id, const diplomatic_trade_t& trade );
	void ClearDiplomaticTrade( const size_t player_id );

	struct diplomatic_loan_offer_t {
		bool proposer_is_lender = true;
		int64_t principal = 0;
		int64_t payment = 0;
		int64_t turns = 0;

		bool operator==( const diplomatic_loan_offer_t& other ) const {
			return
				proposer_is_lender == other.proposer_is_lender &&
				principal == other.principal &&
				payment == other.payment &&
				turns == other.turns;
		}
	};
	using diplomatic_loan_offers_t = std::map< size_t, diplomatic_loan_offer_t >;
	static constexpr size_t MAX_DIPLOMATIC_LOAN_OFFERS = 64;
	static constexpr int64_t MAX_DIPLOMATIC_LOAN_TURNS = 1000;
	const diplomatic_loan_offers_t& GetDiplomaticLoanOffers() const;
	const diplomatic_loan_offer_t* GetDiplomaticLoanOffer( const size_t player_id ) const;
	void SetDiplomaticLoanOffer( const size_t player_id, const diplomatic_loan_offer_t& offer );
	void ClearDiplomaticLoanOffer( const size_t player_id );

	struct diplomatic_loan_t {
		int64_t balance = 0;
		int64_t payment = 0;

		bool operator==( const diplomatic_loan_t& other ) const {
			return balance == other.balance && payment == other.payment;
		}
	};
	using diplomatic_loans_t = std::map< size_t, diplomatic_loan_t >;
	static constexpr size_t MAX_DIPLOMATIC_LOANS = 64;
	const diplomatic_loans_t& GetDiplomaticLoans() const;
	const diplomatic_loan_t* GetDiplomaticLoan( const size_t player_id ) const;
	void SetDiplomaticLoan( const size_t player_id, const diplomatic_loan_t& loan );
	void ClearDiplomaticLoan( const size_t player_id );

	using infiltrated_players_t = std::set< size_t >;
	static constexpr size_t MAX_INFILTRATED_PLAYERS = 64;
	const infiltrated_players_t& GetInfiltratedPlayers() const;
	bool HasInfiltrated( const size_t player_id ) const;
	void SetInfiltrated( const size_t player_id, const bool infiltrated );

	WRAPDEFS_PTR( Player );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;

	WRAPDEF_SERIALIZABLE;

private:

	bool m_is_connected = false;

	std::string m_name = "";
	role_t m_role = PR_NONE;

	slot::Slot* m_slot = nullptr;
	size_t m_slotnum = 0;

	faction::Faction* m_faction = {};
	bool m_owns_faction = false;
	std::string m_difficulty_level = "";

	bool m_is_turn_completed = false;
	technologies_t m_technologies = {};
	std::string m_research_target = "";
	int64_t m_research_progress = 0;
	int64_t m_energy_credits = 0;
	int64_t m_ecological_damage_events = 0;
	int64_t m_major_atrocities = 0;
	int64_t m_sanction_turns = 0;
	int64_t m_integrity_blemishes = 0;
	prototyped_components_t m_prototyped_components = {
		"Infantry", "HandWeapons", "NoArmor", "ColonyModule"
	};
	orbital_facilities_t m_orbital_facilities = {};
	int64_t m_orbital_defense_deployments = 0;
	council_state_t m_council_state = {};
	social_engineering_t m_social_engineering = {{ "Frontier", "Simple", "Survival", "None" }};
	diplomatic_relations_t m_diplomatic_relations = {};
	diplomatic_relations_t m_diplomatic_offers = {};
	infiltrated_players_t m_infiltrated_players = {};
	diplomatic_trades_t m_diplomatic_trades = {};
	diplomatic_loan_offers_t m_diplomatic_loan_offers = {};
	diplomatic_loans_t m_diplomatic_loans = {};

	void ReleaseOwnedFaction();
	static bool ValidateResearchState(
		const technologies_t& technologies,
		const std::string& target,
		const int64_t progress,
		std::string& error
	);
	static bool ValidateSocialEngineering(
		const social_engineering_t& social_engineering,
		std::string& error
	);
	static bool ValidateCouncilState( const council_state_t& state, std::string& error );
};

}
}
