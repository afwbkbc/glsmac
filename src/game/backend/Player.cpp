#include "Player.h"

#include <memory>

#include "game/backend/faction/Faction.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slots.h"

#include "gse/value/String.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/value/Null.h"
#include "gse/value/Array.h"
#include "gse/value/Object.h"

#include "engine/Engine.h"
#include "Game.h"

namespace game {
namespace backend {

Player::Player( types::Buffer buf ) {
	Player::Deserialize( buf );
}

Player::Player(
	const std::string& name,
	const role_t role,
	faction::Faction* faction,
	const std::string& difficulty_level
)
	: m_name( name )
	, m_role( role )
	, m_faction( faction )
	, m_difficulty_level( difficulty_level ) {}

Player::Player( const Player* const other ) {
	m_is_connected = other->m_is_connected;
	m_name = other->m_name;
	m_role = other->m_role;
	m_slot = other->m_slot;
	m_slotnum = other->m_slotnum;
	m_faction = other->m_faction;
	m_difficulty_level = other->m_difficulty_level;
	m_is_turn_completed = other->m_is_turn_completed;
	m_technologies = other->m_technologies;
	m_research_target = other->m_research_target;
	m_research_progress = other->m_research_progress;
	m_energy_credits = other->m_energy_credits;
	m_ecological_damage_events = other->m_ecological_damage_events;
	m_major_atrocities = other->m_major_atrocities;
	m_sanction_turns = other->m_sanction_turns;
	m_integrity_blemishes = other->m_integrity_blemishes;
	m_prototyped_components = other->m_prototyped_components;
	m_orbital_facilities = other->m_orbital_facilities;
	m_orbital_defense_deployments = other->m_orbital_defense_deployments;
	m_council_state = other->m_council_state;
	m_social_engineering = other->m_social_engineering;
	m_diplomatic_relations = other->m_diplomatic_relations;
	m_diplomatic_offers = other->m_diplomatic_offers;
	m_infiltrated_players = other->m_infiltrated_players;
	m_diplomatic_trades = other->m_diplomatic_trades;
	m_diplomatic_loan_offers = other->m_diplomatic_loan_offers;
	m_diplomatic_loans = other->m_diplomatic_loans;
}

Player::~Player() {
	ReleaseOwnedFaction();
}

const std::string& Player::GetPlayerName() const {
	return m_name;
}

const std::string Player::GetFullName() const {
	return GetPlayerName() + " (" + m_faction->m_name + ")";
}

void Player::Connect() {
	ASSERT( !m_is_connected, "player already connected" );
	m_is_connected = true;
}

void Player::Disconnect() {
	ASSERT( m_is_connected, "player not connected" );
	m_is_connected = false;
}

const bool Player::IsConnected() const {
	return m_is_connected;
}

void Player::SetFaction( faction::Faction* faction ) {
	// TODO: validate?
	if ( m_faction != faction ) {
		ReleaseOwnedFaction();
		m_faction = faction;
	}
}

void Player::ClearFaction() {
	ReleaseOwnedFaction();
}

faction::Faction* Player::GetFaction() {
	return m_faction;
}

void Player::SetDifficultyLevel( const std::string& difficulty_level ) {
	// TODO: validate?
	m_difficulty_level = difficulty_level;
}

const std::string& Player::GetDifficultyLevel() const {
	return m_difficulty_level;
}

void Player::SetSlot( slot::Slot* slot ) {
	m_slot = slot;
	if ( m_slot ) {
		m_slotnum = m_slot->GetIndex();
	}
}

slot::Slot* Player::GetSlot() const {
	return m_slot;
}

const Player::role_t Player::GetRole() const {
	return m_role;
}

const bool Player::IsAI() const {
	return m_role == PR_AI;
}

const bool Player::IsNative() const {
	return m_role == PR_NATIVE;
}

const bool Player::IsTurnCompleted() const {
	return m_is_turn_completed;
}

void Player::CompleteTurn() {
	m_is_turn_completed = true;
}

void Player::UncompleteTurn() {
	m_is_turn_completed = false;
}

const Player::technologies_t& Player::GetTechnologies() const {
	return m_technologies;
}

bool Player::HasTechnology( const std::string& id ) const {
	return !id.empty() && m_technologies.find( id ) != m_technologies.end();
}

const std::string& Player::GetResearchTarget() const {
	return m_research_target;
}

int64_t Player::GetResearchProgress() const {
	return m_research_progress;
}

void Player::SetResearchState(
	const technologies_t& technologies,
	const std::string& target,
	const int64_t progress
) {
	std::string error;
	if ( !ValidateResearchState( technologies, target, progress, error ) ) {
		THROW( error );
	}
	m_technologies = technologies;
	m_research_target = target;
	m_research_progress = progress;
}

int64_t Player::GetEnergyCredits() const {
	return m_energy_credits;
}

void Player::SetEnergyCredits( const int64_t energy_credits ) {
	if ( energy_credits < 0 || energy_credits > MAX_ENERGY_CREDITS ) {
		THROW( "player energy credits are out of range" );
	}
	m_energy_credits = energy_credits;
}

int64_t Player::GetEcologicalDamageEvents() const {
	return m_ecological_damage_events;
}

void Player::SetEcologicalDamageEvents( const int64_t ecological_damage_events ) {
	if ( ecological_damage_events < 0 || ecological_damage_events > MAX_ECOLOGICAL_DAMAGE_EVENTS ) {
		THROW( "player ecological damage event count is out of range" );
	}
	m_ecological_damage_events = ecological_damage_events;
}

int64_t Player::GetMajorAtrocities() const {
	return m_major_atrocities;
}

void Player::SetMajorAtrocities( const int64_t major_atrocities ) {
	if ( major_atrocities < 0 || major_atrocities > MAX_MAJOR_ATROCITIES ) {
		THROW( "player major atrocity count is out of range" );
	}
	m_major_atrocities = major_atrocities;
}

int64_t Player::GetSanctionTurns() const {
	return m_sanction_turns;
}

void Player::SetSanctionTurns( const int64_t sanction_turns ) {
	if ( sanction_turns < 0 || sanction_turns > MAX_SANCTION_TURNS ) {
		THROW( "player economic sanction duration is out of range" );
	}
	m_sanction_turns = sanction_turns;
}

int64_t Player::GetIntegrityBlemishes() const {
	return m_integrity_blemishes;
}

void Player::SetIntegrityBlemishes( const int64_t integrity_blemishes ) {
	if ( integrity_blemishes < 0 || integrity_blemishes > MAX_INTEGRITY_BLEMISHES ) {
		THROW( "player diplomatic integrity blemishes are out of range" );
	}
	m_integrity_blemishes = integrity_blemishes;
}

const Player::prototyped_components_t& Player::GetPrototypedComponents() const {
	return m_prototyped_components;
}

bool Player::HasPrototypedComponent( const std::string& id ) const {
	return !id.empty() && m_prototyped_components.find( id ) != m_prototyped_components.end();
}

void Player::SetPrototypedComponents( const prototyped_components_t& components ) {
	if ( components.size() > MAX_PROTOTYPED_COMPONENTS ) {
		THROW( "too many prototyped unit components" );
	}
	for ( const auto& id : components ) {
		if ( id.empty() || id.size() > MAX_PROTOTYPED_COMPONENT_ID_LENGTH ) {
			THROW( "prototyped unit component ID is invalid" );
		}
	}
	m_prototyped_components = components;
}

const Player::orbital_facilities_t& Player::GetOrbitalFacilities() const {
	return m_orbital_facilities;
}

int64_t Player::GetOrbitalFacilityCount( const std::string& id ) const {
	const auto it = m_orbital_facilities.find( id );
	return it == m_orbital_facilities.end() ? 0 : it->second;
}

void Player::SetOrbitalFacilityCount( const std::string& id, const int64_t count ) {
	if ( id.empty() || id.size() > MAX_ORBITAL_FACILITY_ID_LENGTH ) {
		THROW( "orbital facility ID is invalid" );
	}
	if ( count < 0 || count > MAX_ORBITAL_FACILITY_COUNT ) {
		THROW( "orbital facility count is out of range" );
	}
	if ( count == 0 ) {
		m_orbital_facilities.erase( id );
		return;
	}
	if (
		m_orbital_facilities.find( id ) == m_orbital_facilities.end() &&
		m_orbital_facilities.size() >= MAX_ORBITAL_FACILITY_TYPES
	) {
		THROW( "too many orbital facility types" );
	}
	m_orbital_facilities[ id ] = count;
}

int64_t Player::GetOrbitalDefenseDeployments() const {
	return m_orbital_defense_deployments;
}

void Player::SetOrbitalDefenseDeployments( const int64_t deployments ) {
	if ( deployments < 0 || deployments > MAX_ORBITAL_FACILITY_COUNT ) {
		THROW( "orbital defense deployment count is out of range" );
	}
	m_orbital_defense_deployments = deployments;
}

const Player::social_engineering_t& Player::GetSocialEngineering() const {
	return m_social_engineering;
}

void Player::SetSocialEngineering( const social_engineering_t& social_engineering ) {
	std::string error;
	if ( !ValidateSocialEngineering( social_engineering, error ) ) {
		THROW( error );
	}
	m_social_engineering = social_engineering;
}

const Player::diplomatic_relations_t& Player::GetDiplomaticRelations() const {
	return m_diplomatic_relations;
}

Player::diplomatic_relation_t Player::GetDiplomaticRelation( const size_t player_id ) const {
	const auto it = m_diplomatic_relations.find( player_id );
	return it == m_diplomatic_relations.end() ? DR_NEUTRAL : it->second;
}

void Player::SetDiplomaticRelation( const size_t player_id, const diplomatic_relation_t relation ) {
	if ( player_id >= MAX_DIPLOMATIC_RELATIONS ) {
		THROW( "diplomatic relation player ID is out of range" );
	}
	if ( relation < DR_NEUTRAL || relation > DR_VENDETTA ) {
		THROW( "diplomatic relation is invalid" );
	}
	if ( relation == DR_NEUTRAL ) {
		m_diplomatic_relations.erase( player_id );
	}
	else {
		m_diplomatic_relations[ player_id ] = relation;
	}
}

const Player::diplomatic_relations_t& Player::GetDiplomaticOffers() const {
	return m_diplomatic_offers;
}

Player::diplomatic_relation_t Player::GetDiplomaticOffer( const size_t player_id ) const {
	const auto it = m_diplomatic_offers.find( player_id );
	return it == m_diplomatic_offers.end() ? DR_NEUTRAL : it->second;
}

void Player::SetDiplomaticOffer( const size_t player_id, const diplomatic_relation_t relation ) {
	if ( player_id >= MAX_DIPLOMATIC_RELATIONS ) {
		THROW( "diplomatic offer player ID is out of range" );
	}
	if ( relation == DR_NEUTRAL ) {
		m_diplomatic_offers.erase( player_id );
		return;
	}
	if ( relation != DR_TREATY && relation != DR_PACT ) {
		THROW( "diplomatic offer must be a treaty or pact" );
	}
	m_diplomatic_offers[ player_id ] = relation;
}

const std::string Player::GetDiplomaticRelationName( const diplomatic_relation_t relation ) {
	switch ( relation ) {
		case DR_NEUTRAL: return "neutral";
		case DR_TREATY: return "treaty";
		case DR_PACT: return "pact";
		case DR_VENDETTA: return "vendetta";
	}
	THROW( "diplomatic relation is invalid" );
}

bool Player::ParseDiplomaticRelation( const std::string& name, diplomatic_relation_t& relation ) {
	if ( name == "neutral" ) {
		relation = DR_NEUTRAL;
		return true;
	}
	if ( name == "treaty" ) {
		relation = DR_TREATY;
		return true;
	}
	if ( name == "pact" ) {
		relation = DR_PACT;
		return true;
	}
	if ( name == "vendetta" ) {
		relation = DR_VENDETTA;
		return true;
	}
	return false;
}

const Player::diplomatic_trades_t& Player::GetDiplomaticTrades() const {
	return m_diplomatic_trades;
}

const Player::diplomatic_trade_t* Player::GetDiplomaticTrade( const size_t player_id ) const {
	const auto it = m_diplomatic_trades.find( player_id );
	return it == m_diplomatic_trades.end() ? nullptr : &it->second;
}

void Player::SetDiplomaticTrade( const size_t player_id, const diplomatic_trade_t& trade ) {
	if ( player_id >= MAX_DIPLOMATIC_TRADES ) {
		THROW( "diplomatic trade player ID is out of range" );
	}
	if (
		trade.offer_energy < 0 || trade.offer_energy > MAX_ENERGY_CREDITS ||
		trade.request_energy < 0 || trade.request_energy > MAX_ENERGY_CREDITS
	) {
		THROW( "diplomatic trade energy is out of range" );
	}
	if ( trade.offer_energy > 0 && trade.request_energy > 0 ) {
		THROW( "diplomatic trade cannot send energy in both directions" );
	}
	if (
		trade.offer_technology.size() > MAX_DIPLOMATIC_TRADE_TECHNOLOGY_ID_LENGTH ||
		trade.request_technology.size() > MAX_DIPLOMATIC_TRADE_TECHNOLOGY_ID_LENGTH
	) {
		THROW( "diplomatic trade technology ID is too long" );
	}
	if (
		!trade.offer_technology.empty() &&
		trade.offer_technology == trade.request_technology
	) {
		THROW( "diplomatic trade cannot exchange a technology for itself" );
	}
	if (
		trade.offer_energy == 0 && trade.offer_technology.empty() &&
		trade.request_energy == 0 && trade.request_technology.empty()
	) {
		THROW( "diplomatic trade cannot be empty" );
	}
	m_diplomatic_trades[ player_id ] = trade;
}

void Player::ClearDiplomaticTrade( const size_t player_id ) {
	if ( player_id >= MAX_DIPLOMATIC_TRADES ) {
		THROW( "diplomatic trade player ID is out of range" );
	}
	m_diplomatic_trades.erase( player_id );
}

const Player::diplomatic_loan_offers_t& Player::GetDiplomaticLoanOffers() const {
	return m_diplomatic_loan_offers;
}

const Player::diplomatic_loan_offer_t* Player::GetDiplomaticLoanOffer( const size_t player_id ) const {
	const auto it = m_diplomatic_loan_offers.find( player_id );
	return it == m_diplomatic_loan_offers.end() ? nullptr : &it->second;
}

void Player::SetDiplomaticLoanOffer( const size_t player_id, const diplomatic_loan_offer_t& offer ) {
	if ( player_id >= MAX_DIPLOMATIC_LOAN_OFFERS ) {
		THROW( "diplomatic loan offer player ID is out of range" );
	}
	if (
		offer.principal <= 0 || offer.principal > MAX_ENERGY_CREDITS ||
		offer.payment <= 0 || offer.payment > MAX_ENERGY_CREDITS ||
		offer.turns <= 0 || offer.turns > MAX_DIPLOMATIC_LOAN_TURNS ||
		offer.payment > MAX_ENERGY_CREDITS / offer.turns
	) {
		THROW( "diplomatic loan offer terms are out of range" );
	}
	const auto repayment = offer.payment * offer.turns;
	if ( repayment < offer.principal || repayment > offer.principal * 4 ) {
		THROW( "diplomatic loan repayment is outside supported terms" );
	}
	m_diplomatic_loan_offers[ player_id ] = offer;
}

void Player::ClearDiplomaticLoanOffer( const size_t player_id ) {
	if ( player_id >= MAX_DIPLOMATIC_LOAN_OFFERS ) {
		THROW( "diplomatic loan offer player ID is out of range" );
	}
	m_diplomatic_loan_offers.erase( player_id );
}

const Player::diplomatic_loans_t& Player::GetDiplomaticLoans() const {
	return m_diplomatic_loans;
}

const Player::diplomatic_loan_t* Player::GetDiplomaticLoan( const size_t player_id ) const {
	const auto it = m_diplomatic_loans.find( player_id );
	return it == m_diplomatic_loans.end() ? nullptr : &it->second;
}

void Player::SetDiplomaticLoan( const size_t player_id, const diplomatic_loan_t& loan ) {
	if ( player_id >= MAX_DIPLOMATIC_LOANS ) {
		THROW( "diplomatic loan player ID is out of range" );
	}
	if (
		loan.balance <= 0 || loan.balance > MAX_ENERGY_CREDITS ||
		loan.payment <= 0 || loan.payment > MAX_ENERGY_CREDITS
	) {
		THROW( "diplomatic loan is out of range" );
	}
	m_diplomatic_loans[ player_id ] = loan;
}

void Player::ClearDiplomaticLoan( const size_t player_id ) {
	if ( player_id >= MAX_DIPLOMATIC_LOANS ) {
		THROW( "diplomatic loan player ID is out of range" );
	}
	m_diplomatic_loans.erase( player_id );
}

const Player::infiltrated_players_t& Player::GetInfiltratedPlayers() const {
	return m_infiltrated_players;
}

bool Player::HasInfiltrated( const size_t player_id ) const {
	return m_infiltrated_players.find( player_id ) != m_infiltrated_players.end();
}

void Player::SetInfiltrated( const size_t player_id, const bool infiltrated ) {
	if ( player_id >= MAX_INFILTRATED_PLAYERS ) {
		THROW( "infiltrated player ID is out of range" );
	}
	if ( infiltrated ) {
		m_infiltrated_players.insert( player_id );
	}
	else {
		m_infiltrated_players.erase( player_id );
	}
}

const Player::council_state_t& Player::GetCouncilState() const {
	return m_council_state;
}

void Player::SetCouncilState( const council_state_t& state ) {
	std::string error;
	if ( !ValidateCouncilState( state, error ) ) {
		THROW( error );
	}
	m_council_state = state;
}

WRAPIMPL_BEGIN( Player )
	auto* const game = g_engine->GetGame();
	WRAPIMPL_PROPS
			{
				"id",
				VALUE( gse::value::Int, , m_slotnum )
			},
			{
				"type",
				VALUE( gse::value::String, , IsNative() ? "native" : ( IsAI() ? "ai" : "human" ) )
			},
			{
				"name",
				VALUE( gse::value::String, , m_name )
			},
			{
				"difficulty_level",
				VALUE( gse::value::String, , m_difficulty_level )
			},
			{
				"energy_credits",
				VALUE( gse::value::Int, , m_energy_credits )
			},
			{
				"is_ready",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Bool, , m_slot->HasPlayerFlag( ::game::backend::slot::PF_READY ) );
				} )
			},
			{
				"set_ready",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 1 );
					N_GETVALUE( isready, 0, Bool );

					if ( isready ) {
						m_slot->SetPlayerFlag( ::game::backend::slot::PF_READY );
					}
					else {
						m_slot->UnsetPlayerFlag( ::game::backend::slot::PF_READY );
					}

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_faction",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return m_faction
						? m_faction->Wrap( GSE_CALL )
						: VALUE( gse::value::Undefined );
				} )
			},
			{
				"set_faction_by_id",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 1 );
					N_GETVALUE( faction_id, 0, String );
					if ( game->IsStarted() ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Can't change faction after game is started" );
					}
					auto* faction = game->GetFaction( faction_id );
					if ( !faction ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Faction not found: " + faction_id );
					}
					SetFaction( faction );

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"unset_faction",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 0 );
					ClearFaction();

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"is_master",
				VALUE( gse::value::Bool, , m_role == PR_SINGLE || m_role == PR_HOST )
			},
			{
				"get_research_state",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					gse::value::array_elements_t technologies = {};
					technologies.reserve( m_technologies.size() );
					for ( const auto& id : m_technologies ) {
						technologies.push_back( VALUE( gse::value::String, , id ) );
					}
					return VALUEEXT( gse::value::Object, GSE_CALL, gse::value::object_properties_t{
						{ "technologies", VALUE( gse::value::Array, , technologies ) },
						{ "target", VALUE( gse::value::String, , m_research_target ) },
						{ "progress", VALUE( gse::value::Int, , m_research_progress ) },
					} );
				} )
			},
			{
				"set_research_state",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( state, 0, Object );
					N_GETPROP( technology_values, state, "technologies", Array );
					N_GETPROP( target, state, "target", String );
					N_GETPROP( progress, state, "progress", Int );
					technologies_t technologies = {};
					for ( size_t i = 0 ; i < technology_values.size() ; i++ ) {
						N_GETELEMENT( id, technology_values, i, String );
						if ( id.empty() || !technologies.insert( id ).second ) {
							GSE_ERROR( gse::EC.INVALID_CALL, "Research technologies must be unique, non-empty strings" );
						}
					}
					std::string error;
					if ( !ValidateResearchState( technologies, target, progress, error ) ) {
						GSE_ERROR( gse::EC.INVALID_CALL, error );
					}
					SetResearchState( technologies, target, progress );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"has_technology",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( id, 0, String );
					return VALUE( gse::value::Bool, , HasTechnology( id ) );
				} )
			},
			{
				"get_energy_credits",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Int, , GetEnergyCredits() );
				} )
			},
			{
				"set_energy_credits",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( energy_credits, 0, Int );
					if ( energy_credits < 0 || energy_credits > MAX_ENERGY_CREDITS ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Player energy credits are out of range" );
					}
					SetEnergyCredits( energy_credits );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_ecological_damage_events",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Int, , m_ecological_damage_events );
				} )
			},
			{
				"set_ecological_damage_events",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( ecological_damage_events, 0, Int );
					if (
						ecological_damage_events < 0 ||
						ecological_damage_events > MAX_ECOLOGICAL_DAMAGE_EVENTS
					) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Player ecological damage event count is out of range" );
					}
					SetEcologicalDamageEvents( ecological_damage_events );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_major_atrocities",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Int, , m_major_atrocities );
				} )
			},
			{
				"set_major_atrocities",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( major_atrocities, 0, Int );
					if ( major_atrocities < 0 || major_atrocities > MAX_MAJOR_ATROCITIES ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Player major atrocity count is out of range" );
					}
					SetMajorAtrocities( major_atrocities );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_sanction_turns",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Int, , GetSanctionTurns() );
				} )
			},
			{
				"set_sanction_turns",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( sanction_turns, 0, Int );
					try {
						SetSanctionTurns( sanction_turns );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_integrity_blemishes",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Int, , GetIntegrityBlemishes() );
				} )
			},
			{
				"set_integrity_blemishes",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( integrity_blemishes, 0, Int );
					try {
						SetIntegrityBlemishes( integrity_blemishes );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_prototyped_components",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					gse::value::array_elements_t result = {};
					result.reserve( m_prototyped_components.size() );
					for ( const auto& id : m_prototyped_components ) {
						result.push_back( VALUE( gse::value::String, , id ) );
					}
					return VALUE( gse::value::Array, , result );
				} )
			},
			{
				"has_prototyped_component",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( id, 0, String );
					return VALUE( gse::value::Bool, , HasPrototypedComponent( id ) );
				} )
			},
			{
				"set_prototyped_components",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( values, 0, Array );
					prototyped_components_t components = {};
					for ( size_t i = 0 ; i < values.size() ; i++ ) {
						N_GETELEMENT( id, values, i, String );
						if ( !components.insert( id ).second ) {
							GSE_ERROR(
								gse::EC.INVALID_CALL,
								"Prototyped unit components must be unique"
							);
						}
					}
					try {
						SetPrototypedComponents( components );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_orbital_facilities",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					gse::value::object_properties_t result = {};
					for ( const auto& [ id, count ] : m_orbital_facilities ) {
						result.insert( { id, VALUE( gse::value::Int, , count ) } );
					}
					return VALUE( gse::value::Object, , GSE_CALL_NOGC, result );
				} )
			},
			{
				"get_orbital_facility_count",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( id, 0, String );
					return VALUE( gse::value::Int, , GetOrbitalFacilityCount( id ) );
				} )
			},
			{
				"set_orbital_facility_count",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE( id, 0, String );
					N_GETVALUE( count, 1, Int );
					try {
						SetOrbitalFacilityCount( id, count );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_orbital_defense_deployments",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Int, , GetOrbitalDefenseDeployments() );
				} )
			},
			{
				"set_orbital_defense_deployments",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( deployments, 0, Int );
					try {
						SetOrbitalDefenseDeployments( deployments );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_social_engineering",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUEEXT( gse::value::Object, GSE_CALL, gse::value::object_properties_t{
						{ "politics", VALUE( gse::value::String, , m_social_engineering.at( 0 ) ) },
						{ "economics", VALUE( gse::value::String, , m_social_engineering.at( 1 ) ) },
						{ "values", VALUE( gse::value::String, , m_social_engineering.at( 2 ) ) },
						{ "future_society", VALUE( gse::value::String, , m_social_engineering.at( 3 ) ) },
					} );
				} )
			},
			{
				"set_social_engineering",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( choices, 0, Object );
					N_GETPROP( politics, choices, "politics", String );
					N_GETPROP( economics, choices, "economics", String );
					N_GETPROP( values, choices, "values", String );
					N_GETPROP( future_society, choices, "future_society", String );
					social_engineering_t social_engineering = {{
						politics, economics, values, future_society
					}};
					std::string error;
					if ( !ValidateSocialEngineering( social_engineering, error ) ) {
						GSE_ERROR( gse::EC.INVALID_CALL, error );
					}
					SetSocialEngineering( social_engineering );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_diplomatic_relation",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot have a diplomatic relation with itself" );
					}
					return VALUE(
						gse::value::String,
						,
						GetDiplomaticRelationName( GetDiplomaticRelation( other->m_slotnum ) )
					);
				} )
			},
			{
				"set_diplomatic_relation",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					N_GETVALUE( relation_name, 1, String );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot have a diplomatic relation with itself" );
					}
					diplomatic_relation_t relation;
					if ( !ParseDiplomaticRelation( relation_name, relation ) ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Unknown diplomatic relation: " + relation_name );
					}
					SetDiplomaticRelation( other->m_slotnum, relation );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_diplomatic_offer",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot make a diplomatic offer to itself" );
					}
					const auto offer = GetDiplomaticOffer( other->m_slotnum );
					return VALUE(
						gse::value::String,
						,
						offer == DR_NEUTRAL ? "" : GetDiplomaticRelationName( offer )
					);
				} )
			},
			{
				"set_diplomatic_offer",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					N_GETVALUE( offer_name, 1, String );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot make a diplomatic offer to itself" );
					}
					diplomatic_relation_t offer = DR_NEUTRAL;
					if ( !offer_name.empty() && !ParseDiplomaticRelation( offer_name, offer ) ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Unknown diplomatic offer: " + offer_name );
					}
					if ( offer != DR_NEUTRAL && offer != DR_TREATY && offer != DR_PACT ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Diplomatic offer must be a treaty or pact" );
					}
					SetDiplomaticOffer( other->m_slotnum, offer );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_diplomatic_trade",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot trade with itself" );
					}
					const auto* const trade = GetDiplomaticTrade( other->m_slotnum );
					if ( !trade ) {
						return VALUE( gse::value::Null );
					}
					return VALUEEXT( gse::value::Object, GSE_CALL, gse::value::object_properties_t{
						{ "offer_energy", VALUE( gse::value::Int, , trade->offer_energy ) },
						{ "offer_technology", VALUE( gse::value::String, , trade->offer_technology ) },
						{ "request_energy", VALUE( gse::value::Int, , trade->request_energy ) },
						{ "request_technology", VALUE( gse::value::String, , trade->request_technology ) },
					} );
				} )
			},
			{
				"set_diplomatic_trade",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					N_GETVALUE( terms, 1, Object );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot trade with itself" );
					}
					N_GETPROP( offer_energy, terms, "offer_energy", Int );
					N_GETPROP( offer_technology, terms, "offer_technology", String );
					N_GETPROP( request_energy, terms, "request_energy", Int );
					N_GETPROP( request_technology, terms, "request_technology", String );
					try {
						SetDiplomaticTrade( other->m_slotnum, {
							offer_energy,
							offer_technology,
							request_energy,
							request_technology,
						} );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"clear_diplomatic_trade",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot trade with itself" );
					}
					ClearDiplomaticTrade( other->m_slotnum );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_diplomatic_loan_offer",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot negotiate a loan with itself" );
					}
					const auto* const offer = GetDiplomaticLoanOffer( other->m_slotnum );
					if ( !offer ) {
						return VALUE( gse::value::Null );
					}
					return VALUEEXT( gse::value::Object, GSE_CALL, gse::value::object_properties_t{
						{ "proposer_is_lender", VALUE( gse::value::Bool, , offer->proposer_is_lender ) },
						{ "principal", VALUE( gse::value::Int, , offer->principal ) },
						{ "payment", VALUE( gse::value::Int, , offer->payment ) },
						{ "turns", VALUE( gse::value::Int, , offer->turns ) },
					} );
				} )
			},
			{
				"set_diplomatic_loan_offer",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					N_GETVALUE( terms, 1, Object );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot negotiate a loan with itself" );
					}
					N_GETPROP( proposer_is_lender, terms, "proposer_is_lender", Bool );
					N_GETPROP( principal, terms, "principal", Int );
					N_GETPROP( payment, terms, "payment", Int );
					N_GETPROP( turns, terms, "turns", Int );
					try {
						SetDiplomaticLoanOffer( other->m_slotnum, {
							proposer_is_lender,
							principal,
							payment,
							turns,
						} );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"clear_diplomatic_loan_offer",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot negotiate a loan with itself" );
					}
					ClearDiplomaticLoanOffer( other->m_slotnum );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_diplomatic_loan",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( lender, 0, Player );
					if ( lender == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot owe itself a loan" );
					}
					const auto* const loan = GetDiplomaticLoan( lender->m_slotnum );
					if ( !loan ) {
						return VALUE( gse::value::Null );
					}
					return VALUEEXT( gse::value::Object, GSE_CALL, gse::value::object_properties_t{
						{ "balance", VALUE( gse::value::Int, , loan->balance ) },
						{ "payment", VALUE( gse::value::Int, , loan->payment ) },
					} );
				} )
			},
			{
				"set_diplomatic_loan",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE_UNWRAP( lender, 0, Player );
					N_GETVALUE( terms, 1, Object );
					if ( lender == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot owe itself a loan" );
					}
					N_GETPROP( balance, terms, "balance", Int );
					N_GETPROP( payment, terms, "payment", Int );
					try {
						SetDiplomaticLoan( lender->m_slotnum, { balance, payment } );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"clear_diplomatic_loan",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( lender, 0, Player );
					if ( lender == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot owe itself a loan" );
					}
					ClearDiplomaticLoan( lender->m_slotnum );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"has_infiltrated",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot infiltrate itself" );
					}
					return VALUE( gse::value::Bool, , HasInfiltrated( other->m_slotnum ) );
				} )
			},
			{
				"set_infiltrated",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 2 );
					N_GETVALUE_UNWRAP( other, 0, Player );
					N_GETVALUE( infiltrated, 1, Bool );
					if ( other == this ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "A player cannot infiltrate itself" );
					}
					SetInfiltrated( other->m_slotnum, infiltrated );
					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_council_state",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUEEXT( gse::value::Object, GSE_CALL, gse::value::object_properties_t{
						{ "is_governor", VALUE( gse::value::Bool, , m_council_state.is_governor ) },
						{ "last_session_turn", VALUE( gse::value::Int, , m_council_state.last_session_turn ) },
						{ "proposal", VALUE( gse::value::String, , m_council_state.proposal ) },
						{ "caller_id", VALUE( gse::value::Int, , m_council_state.caller_id ) },
						{ "candidate_a_id", VALUE( gse::value::Int, , m_council_state.candidate_a_id ) },
						{ "candidate_b_id", VALUE( gse::value::Int, , m_council_state.candidate_b_id ) },
						{ "vote_id", VALUE( gse::value::Int, , m_council_state.vote_id ) },
						{ "global_trade_pact", VALUE( gse::value::Bool, , m_council_state.global_trade_pact ) },
						{ "unity_core_salvaged", VALUE( gse::value::Bool, , m_council_state.unity_core_salvaged ) },
						{ "un_charter_repealed", VALUE( gse::value::Bool, , m_council_state.un_charter_repealed ) },
					} );
				} )
			},
			{
				"set_council_state",
				NATIVE_CALL( this, game ) {
					game->CheckRW( GSE_CALL );
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( state, 0, Object );
					N_GETPROP( is_governor, state, "is_governor", Bool );
					N_GETPROP( last_session_turn, state, "last_session_turn", Int );
					N_GETPROP( proposal, state, "proposal", String );
					N_GETPROP( caller_id, state, "caller_id", Int );
					N_GETPROP( candidate_a_id, state, "candidate_a_id", Int );
					N_GETPROP( candidate_b_id, state, "candidate_b_id", Int );
					N_GETPROP( vote_id, state, "vote_id", Int );
					N_GETPROP_OPT(
						bool,
						global_trade_pact,
						state,
						"global_trade_pact",
						Bool,
						m_council_state.global_trade_pact
					);
					N_GETPROP_OPT(
						bool,
						unity_core_salvaged,
						state,
						"unity_core_salvaged",
						Bool,
						m_council_state.unity_core_salvaged
					);
					N_GETPROP_OPT(
						bool,
						un_charter_repealed,
						state,
						"un_charter_repealed",
						Bool,
						m_council_state.un_charter_repealed
					);
					try {
						SetCouncilState( {
							is_governor,
							last_session_turn,
							proposal,
							caller_id,
							candidate_a_id,
							candidate_b_id,
							vote_id,
							global_trade_pact,
							unity_core_salvaged,
							un_charter_repealed,
						} );
					}
					catch ( const std::runtime_error& e ) {
						GSE_ERROR( gse::EC.INVALID_CALL, e.what() );
					}
					return VALUE( gse::value::Undefined );
				} )
			},
		};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Player )

const types::Buffer Player::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_name );
	buf.WriteInt( m_role );
	buf.WriteBool( m_faction != nullptr );
	if ( m_faction ) {
		buf.WriteString( m_faction->Serialize().ToString() );
	}
	buf.WriteString( m_difficulty_level );
	buf.WriteBool( m_is_turn_completed );
	buf.WriteInt( m_technologies.size() );
	for ( const auto& id : m_technologies ) {
		buf.WriteString( id );
	}
	buf.WriteString( m_research_target );
	buf.WriteInt( m_research_progress );
	buf.WriteInt( m_energy_credits );
	buf.WriteInt( m_social_engineering.size() );
	for ( const auto& id : m_social_engineering ) {
		buf.WriteString( id );
	}
	buf.WriteInt( m_ecological_damage_events );
	buf.WriteInt( m_diplomatic_relations.size() );
	for ( const auto& [ player_id, relation ] : m_diplomatic_relations ) {
		buf.WriteInt( player_id );
		buf.WriteInt( relation );
	}
	buf.WriteInt( m_diplomatic_offers.size() );
	for ( const auto& [ player_id, relation ] : m_diplomatic_offers ) {
		buf.WriteInt( player_id );
		buf.WriteInt( relation );
	}
	buf.WriteInt( m_infiltrated_players.size() );
	for ( const auto player_id : m_infiltrated_players ) {
		buf.WriteInt( player_id );
	}
	buf.WriteInt( m_major_atrocities );
	buf.WriteInt( m_diplomatic_trades.size() );
	for ( const auto& [ player_id, trade ] : m_diplomatic_trades ) {
		buf.WriteInt( player_id );
		buf.WriteInt( trade.offer_energy );
		buf.WriteString( trade.offer_technology );
		buf.WriteInt( trade.request_energy );
		buf.WriteString( trade.request_technology );
	}
	buf.WriteInt( m_diplomatic_loan_offers.size() );
	for ( const auto& [ player_id, offer ] : m_diplomatic_loan_offers ) {
		buf.WriteInt( player_id );
		buf.WriteBool( offer.proposer_is_lender );
		buf.WriteInt( offer.principal );
		buf.WriteInt( offer.payment );
		buf.WriteInt( offer.turns );
	}
	buf.WriteInt( m_diplomatic_loans.size() );
	for ( const auto& [ player_id, loan ] : m_diplomatic_loans ) {
		buf.WriteInt( player_id );
		buf.WriteInt( loan.balance );
		buf.WriteInt( loan.payment );
	}
	buf.WriteInt( m_sanction_turns );
	buf.WriteInt( m_integrity_blemishes );
	buf.WriteInt( m_prototyped_components.size() );
	for ( const auto& id : m_prototyped_components ) {
		buf.WriteString( id );
	}
	buf.WriteInt( m_orbital_facilities.size() );
	for ( const auto& [ id, count ] : m_orbital_facilities ) {
		buf.WriteString( id );
		buf.WriteInt( count );
	}
	buf.WriteInt( m_orbital_defense_deployments );
	buf.WriteBool( m_council_state.is_governor );
	buf.WriteInt( m_council_state.last_session_turn );
	buf.WriteString( m_council_state.proposal );
	buf.WriteInt( m_council_state.caller_id );
	buf.WriteInt( m_council_state.candidate_a_id );
	buf.WriteInt( m_council_state.candidate_b_id );
	buf.WriteInt( m_council_state.vote_id );
	buf.WriteBool( m_council_state.global_trade_pact );
	buf.WriteBool( m_council_state.unity_core_salvaged );
	buf.WriteBool( m_council_state.un_charter_repealed );

	return buf;
}

void Player::Deserialize( types::Buffer buf ) {

	const auto name = buf.ReadString();
	const auto serialized_role = buf.ReadInt();
	if ( serialized_role < PR_NONE || serialized_role > PR_NATIVE ) {
		THROW( "invalid serialized player role: " + std::to_string( serialized_role ) );
	}
	std::unique_ptr< faction::Faction > faction;
	if ( buf.ReadBool() ) {
		faction = std::make_unique< faction::Faction >();
		faction->Deserialize( buf.ReadString() );
	}
	const auto difficulty_level = buf.ReadString();
	const auto is_turn_completed = buf.ReadBool();
	technologies_t technologies = {};
	const auto technology_count = buf.ReadCollectionSize( "player technology" );
	if ( technology_count > MAX_TECHNOLOGIES ) {
		THROW( "invalid serialized player technology count" );
	}
	for ( size_t i = 0 ; i < technology_count ; i++ ) {
		const auto id = buf.ReadString();
		if ( id.empty() || !technologies.insert( id ).second ) {
			THROW( "invalid or duplicate serialized player technology" );
		}
	}
	const auto research_target = buf.ReadString();
	const auto research_progress = buf.ReadInt();
	std::string research_error;
	if ( !ValidateResearchState( technologies, research_target, research_progress, research_error ) ) {
		THROW( "invalid serialized player research state: " + research_error );
	}
	const auto energy_credits = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	if ( energy_credits < 0 || energy_credits > MAX_ENERGY_CREDITS ) {
		THROW( "invalid serialized player energy credits" );
	}
	social_engineering_t social_engineering = {{ "Frontier", "Simple", "Survival", "None" }};
	if ( buf.GetRemaining() > 0 ) {
		const auto choice_count = buf.ReadCollectionSize( "player social engineering choice" );
		if ( choice_count != SOCIAL_ENGINEERING_CATEGORY_COUNT ) {
			THROW( "invalid serialized player social engineering choice count" );
		}
		for ( size_t i = 0 ; i < choice_count ; i++ ) {
			social_engineering.at( i ) = buf.ReadString();
		}
		std::string social_error;
		if ( !ValidateSocialEngineering( social_engineering, social_error ) ) {
			THROW( "invalid serialized player social engineering state: " + social_error );
		}
	}
	const auto ecological_damage_events = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	if ( ecological_damage_events < 0 || ecological_damage_events > MAX_ECOLOGICAL_DAMAGE_EVENTS ) {
		THROW( "invalid serialized player ecological damage event count" );
	}
	diplomatic_relations_t diplomatic_relations = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto relation_count = buf.ReadCollectionSize( "player diplomatic relation" );
		if ( relation_count > MAX_DIPLOMATIC_RELATIONS ) {
			THROW( "invalid serialized player diplomatic relation count" );
		}
		for ( size_t i = 0 ; i < relation_count ; i++ ) {
			const auto player_id = buf.ReadInt< size_t >( "diplomatic relation player ID" );
			const auto serialized_relation = buf.ReadInt();
			if ( player_id >= MAX_DIPLOMATIC_RELATIONS ) {
				THROW( "invalid serialized diplomatic relation player ID" );
			}
			if ( serialized_relation <= DR_NEUTRAL || serialized_relation > DR_VENDETTA ) {
				THROW( "invalid serialized diplomatic relation" );
			}
			if ( !diplomatic_relations.emplace(
				player_id,
				static_cast< diplomatic_relation_t >( serialized_relation )
			).second ) {
				THROW( "duplicate serialized diplomatic relation player ID" );
			}
		}
	}
	diplomatic_relations_t diplomatic_offers = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto offer_count = buf.ReadCollectionSize( "player diplomatic offer" );
		if ( offer_count > MAX_DIPLOMATIC_RELATIONS ) {
			THROW( "invalid serialized player diplomatic offer count" );
		}
		for ( size_t i = 0 ; i < offer_count ; i++ ) {
			const auto player_id = buf.ReadInt< size_t >( "diplomatic offer player ID" );
			const auto serialized_offer = buf.ReadInt();
			if ( player_id >= MAX_DIPLOMATIC_RELATIONS ) {
				THROW( "invalid serialized diplomatic offer player ID" );
			}
			if ( serialized_offer != DR_TREATY && serialized_offer != DR_PACT ) {
				THROW( "invalid serialized diplomatic offer" );
			}
			if ( !diplomatic_offers.emplace(
				player_id,
				static_cast< diplomatic_relation_t >( serialized_offer )
			).second ) {
				THROW( "duplicate serialized diplomatic offer player ID" );
			}
		}
	}
	infiltrated_players_t infiltrated_players = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto infiltration_count = buf.ReadCollectionSize( "player infiltration" );
		if ( infiltration_count > MAX_INFILTRATED_PLAYERS ) {
			THROW( "invalid serialized player infiltration count" );
		}
		for ( size_t i = 0 ; i < infiltration_count ; i++ ) {
			const auto player_id = buf.ReadInt< size_t >( "infiltrated player ID" );
			if ( player_id >= MAX_INFILTRATED_PLAYERS ) {
				THROW( "invalid serialized infiltrated player ID" );
			}
			if ( !infiltrated_players.insert( player_id ).second ) {
				THROW( "duplicate serialized infiltrated player ID" );
			}
		}
	}
	const auto major_atrocities = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	if ( major_atrocities < 0 || major_atrocities > MAX_MAJOR_ATROCITIES ) {
		THROW( "invalid serialized player major atrocity count" );
	}
	diplomatic_trades_t diplomatic_trades = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto trade_count = buf.ReadCollectionSize( "player diplomatic trade" );
		if ( trade_count > MAX_DIPLOMATIC_TRADES ) {
			THROW( "invalid serialized player diplomatic trade count" );
		}
		for ( size_t i = 0 ; i < trade_count ; i++ ) {
			const auto player_id = buf.ReadInt< size_t >( "diplomatic trade player ID" );
			const diplomatic_trade_t trade = {
				buf.ReadInt(),
				buf.ReadString(),
				buf.ReadInt(),
				buf.ReadString(),
			};
			Player validator( "trade validator", PR_NONE, nullptr, "" );
			validator.SetDiplomaticTrade( player_id, trade );
			if ( !diplomatic_trades.emplace( player_id, trade ).second ) {
				THROW( "duplicate serialized diplomatic trade player ID" );
			}
		}
	}
	diplomatic_loan_offers_t diplomatic_loan_offers = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto offer_count = buf.ReadCollectionSize( "player diplomatic loan offer" );
		if ( offer_count > MAX_DIPLOMATIC_LOAN_OFFERS ) {
			THROW( "invalid serialized player diplomatic loan offer count" );
		}
		for ( size_t i = 0 ; i < offer_count ; i++ ) {
			const auto player_id = buf.ReadInt< size_t >( "diplomatic loan offer player ID" );
			const diplomatic_loan_offer_t offer = {
				buf.ReadBool(),
				buf.ReadInt(),
				buf.ReadInt(),
				buf.ReadInt(),
			};
			Player validator( "loan offer validator", PR_NONE, nullptr, "" );
			validator.SetDiplomaticLoanOffer( player_id, offer );
			if ( !diplomatic_loan_offers.emplace( player_id, offer ).second ) {
				THROW( "duplicate serialized diplomatic loan offer player ID" );
			}
		}
	}
	diplomatic_loans_t diplomatic_loans = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto loan_count = buf.ReadCollectionSize( "player diplomatic loan" );
		if ( loan_count > MAX_DIPLOMATIC_LOANS ) {
			THROW( "invalid serialized player diplomatic loan count" );
		}
		for ( size_t i = 0 ; i < loan_count ; i++ ) {
			const auto player_id = buf.ReadInt< size_t >( "diplomatic loan player ID" );
			const diplomatic_loan_t loan = { buf.ReadInt(), buf.ReadInt() };
			Player validator( "loan validator", PR_NONE, nullptr, "" );
			validator.SetDiplomaticLoan( player_id, loan );
			if ( !diplomatic_loans.emplace( player_id, loan ).second ) {
				THROW( "duplicate serialized diplomatic loan player ID" );
			}
		}
	}
	int64_t sanction_turns = 0;
	if ( buf.GetRemaining() > 0 ) {
		sanction_turns = buf.ReadInt();
		if ( sanction_turns < 0 || sanction_turns > MAX_SANCTION_TURNS ) {
			THROW( "invalid serialized player economic sanction duration" );
		}
	}
	int64_t integrity_blemishes = 0;
	if ( buf.GetRemaining() > 0 ) {
		integrity_blemishes = buf.ReadInt();
		if ( integrity_blemishes < 0 || integrity_blemishes > MAX_INTEGRITY_BLEMISHES ) {
			THROW( "invalid serialized player diplomatic integrity blemishes" );
		}
	}
	prototyped_components_t prototyped_components = {
		"Infantry", "HandWeapons", "NoArmor", "ColonyModule"
	};
	if ( buf.GetRemaining() > 0 ) {
		prototyped_components.clear();
		const auto component_count = buf.ReadCollectionSize( "prototyped unit component" );
		if ( component_count > MAX_PROTOTYPED_COMPONENTS ) {
			THROW( "invalid serialized prototyped unit component count" );
		}
		for ( size_t i = 0 ; i < component_count ; i++ ) {
			const auto id = buf.ReadString();
			if (
				id.empty() || id.size() > MAX_PROTOTYPED_COMPONENT_ID_LENGTH ||
				!prototyped_components.insert( id ).second
			) {
				THROW( "invalid or duplicate serialized prototyped unit component" );
			}
		}
	}
	orbital_facilities_t orbital_facilities = {};
	if ( buf.GetRemaining() > 0 ) {
		const auto orbital_count = buf.ReadCollectionSize( "player orbital facility" );
		if ( orbital_count > MAX_ORBITAL_FACILITY_TYPES ) {
			THROW( "invalid serialized player orbital facility type count" );
		}
		for ( size_t i = 0 ; i < orbital_count ; i++ ) {
			const auto id = buf.ReadString();
			const auto count = buf.ReadInt();
			if (
				id.empty() || id.size() > MAX_ORBITAL_FACILITY_ID_LENGTH ||
				count <= 0 || count > MAX_ORBITAL_FACILITY_COUNT ||
				!orbital_facilities.emplace( id, count ).second
			) {
				THROW( "invalid or duplicate serialized player orbital facility" );
			}
		}
	}
	int64_t orbital_defense_deployments = 0;
	if ( buf.GetRemaining() > 0 ) {
		orbital_defense_deployments = buf.ReadInt();
		if (
			orbital_defense_deployments < 0 ||
			orbital_defense_deployments > MAX_ORBITAL_FACILITY_COUNT
		) {
			THROW( "invalid serialized orbital defense deployment count" );
		}
	}
	council_state_t council_state = {};
	if ( buf.GetRemaining() > 0 ) {
		council_state = {
			buf.ReadBool(),
			buf.ReadInt(),
			buf.ReadString(),
			buf.ReadInt(),
			buf.ReadInt(),
			buf.ReadInt(),
			buf.ReadInt(),
			false,
			false,
			false,
		};
		if ( buf.GetRemaining() > 0 ) {
			council_state.global_trade_pact = buf.ReadBool();
		}
		if ( buf.GetRemaining() > 0 ) {
			council_state.unity_core_salvaged = buf.ReadBool();
		}
		if ( buf.GetRemaining() > 0 ) {
			council_state.un_charter_repealed = buf.ReadBool();
		}
		std::string council_error;
		if ( !ValidateCouncilState( council_state, council_error ) ) {
			THROW( "invalid serialized Planetary Council state: " + council_error );
		}
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized player" );
	}

	ReleaseOwnedFaction();
	m_name = name;
	m_role = static_cast< role_t >( serialized_role );
	m_faction = faction.release();
	m_owns_faction = m_faction != nullptr;
	m_difficulty_level = difficulty_level;
	m_is_turn_completed = is_turn_completed;
	m_technologies = std::move( technologies );
	m_research_target = research_target;
	m_research_progress = research_progress;
	m_energy_credits = energy_credits;
	m_ecological_damage_events = ecological_damage_events;
	m_social_engineering = std::move( social_engineering );
	m_diplomatic_relations = std::move( diplomatic_relations );
	m_diplomatic_offers = std::move( diplomatic_offers );
	m_infiltrated_players = std::move( infiltrated_players );
	m_major_atrocities = major_atrocities;
	m_diplomatic_trades = std::move( diplomatic_trades );
	m_diplomatic_loan_offers = std::move( diplomatic_loan_offers );
	m_diplomatic_loans = std::move( diplomatic_loans );
	m_sanction_turns = sanction_turns;
	m_integrity_blemishes = integrity_blemishes;
	m_prototyped_components = std::move( prototyped_components );
	m_orbital_facilities = std::move( orbital_facilities );
	m_orbital_defense_deployments = orbital_defense_deployments;
	m_council_state = std::move( council_state );

}

void Player::ReleaseOwnedFaction() {
	if ( m_owns_faction ) {
		delete m_faction;
	}
	m_faction = nullptr;
	m_owns_faction = false;
}

bool Player::ValidateResearchState(
	const technologies_t& technologies,
	const std::string& target,
	const int64_t progress,
	std::string& error
) {
	if ( technologies.size() > MAX_TECHNOLOGIES ) {
		error = "Too many researched technologies";
		return false;
	}
	for ( const auto& id : technologies ) {
		if ( id.empty() ) {
			error = "Researched technology IDs cannot be empty";
			return false;
		}
	}
	if ( !target.empty() && technologies.find( target ) != technologies.end() ) {
		error = "Research target is already known";
		return false;
	}
	if ( progress < 0 || progress > MAX_RESEARCH_PROGRESS ) {
		error = "Research progress is out of range";
		return false;
	}
	if ( target.empty() && progress != 0 ) {
		error = "Research progress requires a target";
		return false;
	}
	return true;
}

bool Player::ValidateSocialEngineering(
	const social_engineering_t& social_engineering,
	std::string& error
) {
	for ( const auto& id : social_engineering ) {
		if ( id.empty() ) {
			error = "Social engineering choice IDs cannot be empty";
			return false;
		}
		if ( id.size() > MAX_SOCIAL_ENGINEERING_ID_LENGTH ) {
			error = "Social engineering choice ID is too long";
			return false;
		}
	}
	return true;
}

bool Player::ValidateCouncilState( const council_state_t& state, std::string& error ) {
	if ( state.last_session_turn < 0 || state.last_session_turn > MAX_COUNCIL_TURN ) {
		error = "Planetary Council session turn is out of range";
		return false;
	}
	if ( state.proposal.empty() ) {
		if (
			state.caller_id != -1 || state.candidate_a_id != -1 ||
			state.candidate_b_id != -1 || state.vote_id != COUNCIL_VOTE_PENDING
		) {
			error = "Inactive Planetary Council state contains session data";
			return false;
		}
		return true;
	}
	const bool is_policy =
		state.proposal == "trade_pact" || state.proposal == "repeal_trade_pact" ||
		state.proposal == "salvage_unity_core" || state.proposal == "repeal_un_charter" ||
		state.proposal == "reinstate_un_charter";
	if ( state.proposal != "governor" && state.proposal != "supreme" && !is_policy ) {
		error = "Planetary Council proposal is unsupported";
		return false;
	}
	if ( state.last_session_turn <= 0 ) {
		error = "Active Planetary Council session requires a positive turn";
		return false;
	}
	const auto valid_player_id = []( const int64_t id ) {
		return id >= 0 && id < static_cast< int64_t >( MAX_COUNCIL_PLAYER_ID );
	};
	if ( !valid_player_id( state.caller_id ) ) {
		error = "Planetary Council participant ID is invalid";
		return false;
	}
	if ( is_policy ) {
		if (
			state.candidate_a_id != COUNCIL_VOTE_YES ||
			state.candidate_b_id != COUNCIL_VOTE_NO
		) {
			error = "Planetary Council policy choices are invalid";
			return false;
		}
	}
	else if (
		!valid_player_id( state.candidate_a_id ) ||
		!valid_player_id( state.candidate_b_id ) ||
		state.candidate_a_id == state.candidate_b_id
	) {
		error = "Planetary Council participant ID is invalid";
		return false;
	}
	if (
		state.vote_id != COUNCIL_VOTE_PENDING && state.vote_id != COUNCIL_VOTE_ABSTAIN &&
		state.vote_id != state.candidate_a_id && state.vote_id != state.candidate_b_id
	) {
		error = "Planetary Council vote is invalid";
		return false;
	}
	return true;
}

WRAPIMPL_SERIALIZE( Player )
	buf->WriteInt( obj->m_slotnum );
}

WRAPIMPL_DESERIALIZE( Player )
	const auto slot_num = buf->ReadInt< size_t >( "player reference slot" );
	auto* const slots = game->GetState()->m_slots;
	if ( slot_num >= slots->GetCount() ) {
		THROW( "player reference slot is out of bounds: " + std::to_string( slot_num ) );
	}
	auto& player_slot = slots->GetSlot( slot_num );
	if ( player_slot.GetState() != slot::Slot::SS_PLAYER ) {
		THROW( "player reference points to an empty slot: " + std::to_string( slot_num ) );
	}
	auto* const player = player_slot.GetPlayer();
	if ( !player ) {
		THROW( "player reference points to a slot without a player: " + std::to_string( slot_num ) );
	}
	return player->Wrap( GSE_CALL );
}

}
}
