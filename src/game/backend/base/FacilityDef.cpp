#include "FacilityDef.h"

#include "gse/value/Bool.h"
#include "gse/value/Float.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"

namespace game {
namespace backend {
namespace base {

FacilityDef::FacilityDef(
	const std::string& id,
	const std::string& name,
	const int64_t mineral_cost,
	const int64_t nutrient_bonus,
	const int64_t mineral_bonus,
	const int64_t energy_bonus,
	const int64_t energy_maintenance,
	const std::string& required_technology,
	const int64_t psych_bonus,
	const float research_multiplier,
	const float defense_multiplier,
	const float economy_multiplier,
	const int64_t unit_morale_bonus,
	const int64_t research_bonus,
	const float mineral_multiplier,
	const float psych_multiplier,
	const int64_t population_limit,
	const std::string& required_facility,
	const int64_t drone_modifier,
	const int64_t talent_bonus,
	const bool suppress_psych,
	const int64_t unit_morale_land_bonus,
	const int64_t unit_morale_water_bonus,
	const int64_t unit_morale_air_bonus,
	const float water_defense_multiplier,
	const float air_defense_multiplier,
	const int64_t growth_rating_bonus,
	const int64_t native_lifecycle_bonus,
	const bool is_project,
	const std::string& granted_facility,
	const int64_t global_talent_bonus,
	const int64_t global_growth_rating_bonus,
	const int64_t global_population_limit_bonus,
	const int64_t global_mineral_bonus,
	const int64_t global_support_bonus,
	const float global_maintenance_multiplier,
	const int64_t global_native_lifecycle_bonus,
	const int64_t network_node_drone_modifier,
	const int64_t network_node_research_bonus,
	const int64_t worked_tile_energy_bonus,
	const bool global_prevent_riots,
	const float global_terraforming_rate_multiplier,
	const int64_t new_base_population,
	const int64_t small_base_drone_modifier,
	const float global_psi_attack_multiplier,
	const float global_psi_defense_multiplier,
	const float global_naval_movement_bonus,
	const bool global_full_repair,
	const std::string& required_project,
	const int64_t forest_nutrient_bonus,
	const int64_t forest_mineral_bonus,
	const int64_t forest_energy_bonus,
	const bool full_repair_land,
	const bool full_repair_water,
	const bool full_repair_air,
	const bool full_repair_native,
	const int64_t defender_morale_bonus,
	const int64_t global_police_rating_bonus,
	const int64_t global_extra_police_units,
	const int64_t efficiency_rating_bonus,
	const int64_t defender_morale_minimum,
	const bool prototype_cost_waiver,
	const int64_t mineral_to_energy_divisor,
	const std::string& orbital_resource,
	const bool orbital_defense,
	const bool psi_gate
)
	: m_id( id )
	, m_name( name )
	, m_mineral_cost( mineral_cost )
	, m_nutrient_bonus( nutrient_bonus )
	, m_mineral_bonus( mineral_bonus )
	, m_energy_bonus( energy_bonus )
	, m_energy_maintenance( energy_maintenance )
	, m_required_technology( required_technology )
	, m_psych_bonus( psych_bonus )
	, m_research_multiplier( research_multiplier )
	, m_defense_multiplier( defense_multiplier )
	, m_economy_multiplier( economy_multiplier )
	, m_unit_morale_bonus( unit_morale_bonus )
	, m_research_bonus( research_bonus )
	, m_mineral_multiplier( mineral_multiplier )
	, m_psych_multiplier( psych_multiplier )
	, m_population_limit( population_limit )
	, m_required_facility( required_facility )
	, m_drone_modifier( drone_modifier )
	, m_talent_bonus( talent_bonus )
	, m_suppress_psych( suppress_psych )
	, m_unit_morale_land_bonus( unit_morale_land_bonus )
	, m_unit_morale_water_bonus( unit_morale_water_bonus )
	, m_unit_morale_air_bonus( unit_morale_air_bonus )
	, m_water_defense_multiplier( water_defense_multiplier )
	, m_air_defense_multiplier( air_defense_multiplier )
	, m_growth_rating_bonus( growth_rating_bonus )
	, m_native_lifecycle_bonus( native_lifecycle_bonus )
	, m_is_project( is_project )
	, m_granted_facility( granted_facility )
	, m_global_talent_bonus( global_talent_bonus )
	, m_global_growth_rating_bonus( global_growth_rating_bonus )
	, m_global_population_limit_bonus( global_population_limit_bonus )
	, m_global_mineral_bonus( global_mineral_bonus )
	, m_global_support_bonus( global_support_bonus )
	, m_global_maintenance_multiplier( global_maintenance_multiplier )
	, m_global_native_lifecycle_bonus( global_native_lifecycle_bonus )
	, m_network_node_drone_modifier( network_node_drone_modifier )
	, m_network_node_research_bonus( network_node_research_bonus )
	, m_worked_tile_energy_bonus( worked_tile_energy_bonus )
	, m_global_prevent_riots( global_prevent_riots )
	, m_global_terraforming_rate_multiplier( global_terraforming_rate_multiplier )
	, m_new_base_population( new_base_population )
	, m_small_base_drone_modifier( small_base_drone_modifier )
	, m_global_psi_attack_multiplier( global_psi_attack_multiplier )
	, m_global_psi_defense_multiplier( global_psi_defense_multiplier )
	, m_global_naval_movement_bonus( global_naval_movement_bonus )
	, m_global_full_repair( global_full_repair )
	, m_required_project( required_project )
	, m_forest_nutrient_bonus( forest_nutrient_bonus )
	, m_forest_mineral_bonus( forest_mineral_bonus )
	, m_forest_energy_bonus( forest_energy_bonus )
	, m_full_repair_land( full_repair_land )
	, m_full_repair_water( full_repair_water )
	, m_full_repair_air( full_repair_air )
	, m_full_repair_native( full_repair_native )
	, m_defender_morale_bonus( defender_morale_bonus )
	, m_global_police_rating_bonus( global_police_rating_bonus )
	, m_global_extra_police_units( global_extra_police_units )
	, m_efficiency_rating_bonus( efficiency_rating_bonus )
	, m_defender_morale_minimum( defender_morale_minimum )
	, m_prototype_cost_waiver( prototype_cost_waiver )
	, m_mineral_to_energy_divisor( mineral_to_energy_divisor )
	, m_orbital_resource( orbital_resource )
	, m_orbital_defense( orbital_defense )
	, m_psi_gate( psi_gate ) {
	if (
		m_id.empty() ||
		m_name.empty() ||
		m_mineral_cost < 0 ||
		( m_mineral_cost == 0 && m_mineral_to_energy_divisor == 0 ) ||
		m_mineral_cost > MAX_MINERAL_COST ||
		m_nutrient_bonus < 0 ||
		m_nutrient_bonus > MAX_RESOURCE_BONUS ||
		m_mineral_bonus < 0 ||
		m_mineral_bonus > MAX_RESOURCE_BONUS ||
		m_energy_bonus < 0 ||
		m_energy_bonus > MAX_RESOURCE_BONUS ||
		m_energy_maintenance < 0 ||
		m_energy_maintenance > MAX_ENERGY_MAINTENANCE ||
		m_psych_bonus < 0 ||
		m_psych_bonus > MAX_RESOURCE_BONUS ||
		m_research_multiplier < MIN_RESEARCH_MULTIPLIER ||
		m_research_multiplier > MAX_RESEARCH_MULTIPLIER ||
		m_defense_multiplier < 1.0f ||
		m_defense_multiplier > MAX_DEFENSE_MULTIPLIER ||
		m_economy_multiplier < 0.0f ||
		m_economy_multiplier > MAX_ECONOMY_MULTIPLIER ||
		m_unit_morale_bonus < 0 ||
		m_unit_morale_bonus > MAX_UNIT_MORALE_BONUS ||
		m_research_bonus < 0 ||
		m_research_bonus > MAX_RESOURCE_BONUS ||
		m_mineral_multiplier < 0.0f ||
		m_mineral_multiplier > MAX_MINERAL_MULTIPLIER ||
		m_psych_multiplier < 0.0f ||
		m_psych_multiplier > MAX_PSYCH_MULTIPLIER ||
		m_population_limit < 0 ||
		m_population_limit > MAX_POPULATION_LIMIT ||
		m_required_facility == m_id ||
		m_required_project == m_id ||
		m_drone_modifier < -MAX_DRONE_MODIFIER ||
		m_drone_modifier > MAX_DRONE_MODIFIER ||
		m_talent_bonus < 0 ||
		m_talent_bonus > MAX_RESOURCE_BONUS ||
		m_unit_morale_land_bonus < 0 ||
		m_unit_morale_land_bonus > MAX_UNIT_MORALE_BONUS ||
		m_unit_morale_water_bonus < 0 ||
		m_unit_morale_water_bonus > MAX_UNIT_MORALE_BONUS ||
		m_unit_morale_air_bonus < 0 ||
		m_unit_morale_air_bonus > MAX_UNIT_MORALE_BONUS ||
		m_water_defense_multiplier < 1.0f ||
		m_water_defense_multiplier > MAX_DEFENSE_MULTIPLIER ||
		m_air_defense_multiplier < 1.0f ||
		m_air_defense_multiplier > MAX_DEFENSE_MULTIPLIER ||
		m_growth_rating_bonus < 0 ||
		m_growth_rating_bonus > MAX_GROWTH_RATING_BONUS ||
		m_native_lifecycle_bonus < 0 ||
		m_native_lifecycle_bonus > MAX_UNIT_MORALE_BONUS ||
		m_forest_nutrient_bonus < 0 ||
		m_forest_nutrient_bonus > MAX_RESOURCE_BONUS ||
		m_forest_mineral_bonus < 0 ||
		m_forest_mineral_bonus > MAX_RESOURCE_BONUS ||
		m_forest_energy_bonus < 0 ||
		m_forest_energy_bonus > MAX_RESOURCE_BONUS ||
		m_defender_morale_bonus < 0 ||
		m_defender_morale_bonus > MAX_UNIT_MORALE_BONUS ||
		m_global_police_rating_bonus < 0 ||
		m_global_police_rating_bonus > MAX_UNIT_MORALE_BONUS ||
		m_global_extra_police_units < 0 ||
		m_global_extra_police_units > MAX_UNIT_MORALE_BONUS ||
		m_efficiency_rating_bonus < 0 ||
		m_efficiency_rating_bonus > MAX_GROWTH_RATING_BONUS ||
		m_defender_morale_minimum < 0 ||
		m_defender_morale_minimum > MAX_UNIT_MORALE_BONUS ||
		m_mineral_to_energy_divisor < 0 ||
		m_mineral_to_energy_divisor > MAX_RESOURCE_BONUS ||
		(
			m_mineral_to_energy_divisor > 0 &&
			( m_is_project || m_mineral_cost != 0 )
		) ||
		(
			!m_orbital_resource.empty() &&
			m_orbital_resource != "NUTRIENTS" &&
			m_orbital_resource != "MINERALS" &&
			m_orbital_resource != "ENERGY"
		) ||
		( !m_orbital_resource.empty() && m_orbital_defense ) ||
		( ( !m_orbital_resource.empty() || m_orbital_defense ) && m_is_project ) ||
		!m_is_project && (
			!m_granted_facility.empty() ||
			m_global_talent_bonus != 0 ||
			m_global_growth_rating_bonus != 0 ||
			m_global_population_limit_bonus != 0 ||
			m_global_mineral_bonus != 0 ||
			m_global_support_bonus != 0 ||
			m_global_maintenance_multiplier != 1.0f ||
			m_global_native_lifecycle_bonus != 0 ||
			m_network_node_drone_modifier != 0 ||
			m_network_node_research_bonus != 0 ||
			m_worked_tile_energy_bonus != 0 ||
			m_global_prevent_riots ||
			m_global_terraforming_rate_multiplier != 1.0f ||
			m_new_base_population != 0 ||
			m_small_base_drone_modifier != 0 ||
			m_global_psi_attack_multiplier != 1.0f ||
			m_global_psi_defense_multiplier != 1.0f ||
			m_global_naval_movement_bonus != 0.0f ||
			m_global_full_repair ||
			m_global_police_rating_bonus != 0 ||
			m_global_extra_police_units != 0
		) ||
		m_global_talent_bonus < 0 ||
		m_global_talent_bonus > MAX_RESOURCE_BONUS ||
		m_global_growth_rating_bonus < 0 ||
		m_global_growth_rating_bonus > MAX_GROWTH_RATING_BONUS ||
		m_global_population_limit_bonus < 0 ||
		m_global_population_limit_bonus > MAX_POPULATION_LIMIT ||
		m_global_mineral_bonus < 0 ||
		m_global_mineral_bonus > MAX_RESOURCE_BONUS ||
		m_global_support_bonus < 0 ||
		m_global_support_bonus > MAX_RESOURCE_BONUS ||
		m_global_maintenance_multiplier < 0.0f ||
		m_global_maintenance_multiplier > 1.0f ||
		m_global_native_lifecycle_bonus < 0 ||
		m_global_native_lifecycle_bonus > MAX_UNIT_MORALE_BONUS ||
		m_network_node_drone_modifier < -MAX_DRONE_MODIFIER ||
		m_network_node_drone_modifier > MAX_DRONE_MODIFIER ||
		m_network_node_research_bonus < 0 ||
		m_network_node_research_bonus > MAX_RESOURCE_BONUS ||
		m_worked_tile_energy_bonus < 0 ||
		m_worked_tile_energy_bonus > MAX_RESOURCE_BONUS ||
		m_global_terraforming_rate_multiplier < 1.0f ||
		m_global_terraforming_rate_multiplier > MAX_DEFENSE_MULTIPLIER ||
		m_new_base_population < 0 ||
		m_new_base_population > MAX_POPULATION_LIMIT ||
		m_small_base_drone_modifier < -MAX_DRONE_MODIFIER ||
		m_small_base_drone_modifier > MAX_DRONE_MODIFIER ||
		m_global_psi_attack_multiplier < 1.0f ||
		m_global_psi_attack_multiplier > MAX_DEFENSE_MULTIPLIER ||
		m_global_psi_defense_multiplier < 1.0f ||
		m_global_psi_defense_multiplier > MAX_DEFENSE_MULTIPLIER ||
		m_global_naval_movement_bonus < 0.0f ||
		m_global_naval_movement_bonus > MAX_DEFENSE_MULTIPLIER
	) {
		THROW( "invalid base facility definition: " + m_id );
	}
}

const types::Buffer FacilityDef::Serialize( const FacilityDef* def ) {
	types::Buffer buf;
	buf.WriteString( def->m_id );
	buf.WriteString( def->m_name );
	buf.WriteInt( def->m_mineral_cost );
	buf.WriteInt( def->m_nutrient_bonus );
	buf.WriteInt( def->m_mineral_bonus );
	buf.WriteInt( def->m_energy_bonus );
	buf.WriteInt( def->m_energy_maintenance );
	buf.WriteString( def->m_required_technology );
	buf.WriteInt( def->m_psych_bonus );
	buf.WriteFloat( def->m_research_multiplier );
	buf.WriteFloat( def->m_defense_multiplier );
	buf.WriteFloat( def->m_economy_multiplier );
	buf.WriteInt( def->m_unit_morale_bonus );
	buf.WriteInt( def->m_research_bonus );
	buf.WriteFloat( def->m_mineral_multiplier );
	buf.WriteFloat( def->m_psych_multiplier );
	buf.WriteInt( def->m_population_limit );
	buf.WriteString( def->m_required_facility );
	buf.WriteInt( def->m_drone_modifier );
	buf.WriteInt( def->m_talent_bonus );
	buf.WriteBool( def->m_suppress_psych );
	buf.WriteInt( def->m_unit_morale_land_bonus );
	buf.WriteInt( def->m_unit_morale_water_bonus );
	buf.WriteInt( def->m_unit_morale_air_bonus );
	buf.WriteFloat( def->m_water_defense_multiplier );
	buf.WriteFloat( def->m_air_defense_multiplier );
	buf.WriteInt( def->m_growth_rating_bonus );
	buf.WriteInt( def->m_native_lifecycle_bonus );
	buf.WriteBool( def->m_is_project );
	buf.WriteString( def->m_granted_facility );
	buf.WriteInt( def->m_global_talent_bonus );
	buf.WriteInt( def->m_global_growth_rating_bonus );
	buf.WriteInt( def->m_global_population_limit_bonus );
	buf.WriteInt( def->m_global_mineral_bonus );
	buf.WriteInt( def->m_global_support_bonus );
	buf.WriteFloat( def->m_global_maintenance_multiplier );
	buf.WriteInt( def->m_global_native_lifecycle_bonus );
	buf.WriteInt( def->m_network_node_drone_modifier );
	buf.WriteInt( def->m_network_node_research_bonus );
	buf.WriteInt( def->m_worked_tile_energy_bonus );
	buf.WriteBool( def->m_global_prevent_riots );
	buf.WriteFloat( def->m_global_terraforming_rate_multiplier );
	buf.WriteInt( def->m_new_base_population );
	buf.WriteInt( def->m_small_base_drone_modifier );
	buf.WriteFloat( def->m_global_psi_attack_multiplier );
	buf.WriteFloat( def->m_global_psi_defense_multiplier );
	buf.WriteFloat( def->m_global_naval_movement_bonus );
	buf.WriteBool( def->m_global_full_repair );
	buf.WriteString( def->m_required_project );
	buf.WriteInt( def->m_forest_nutrient_bonus );
	buf.WriteInt( def->m_forest_mineral_bonus );
	buf.WriteInt( def->m_forest_energy_bonus );
	buf.WriteBool( def->m_full_repair_land );
	buf.WriteBool( def->m_full_repair_water );
	buf.WriteBool( def->m_full_repair_air );
	buf.WriteBool( def->m_full_repair_native );
	buf.WriteInt( def->m_defender_morale_bonus );
	buf.WriteInt( def->m_global_police_rating_bonus );
	buf.WriteInt( def->m_global_extra_police_units );
	buf.WriteInt( def->m_efficiency_rating_bonus );
	buf.WriteInt( def->m_defender_morale_minimum );
	buf.WriteBool( def->m_prototype_cost_waiver );
	buf.WriteInt( def->m_mineral_to_energy_divisor );
	buf.WriteString( def->m_orbital_resource );
	buf.WriteBool( def->m_orbital_defense );
	buf.WriteBool( def->m_psi_gate );
	return buf;
}

FacilityDef* FacilityDef::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto name = buf.ReadString();
	const auto mineral_cost = buf.ReadInt();
	const auto nutrient_bonus = buf.ReadInt();
	const auto mineral_bonus = buf.ReadInt();
	const auto energy_bonus = buf.ReadInt();
	const auto energy_maintenance = buf.ReadInt();
	const auto required_technology = buf.GetRemaining() > 0 ? buf.ReadString() : "";
	const auto psych_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto research_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 0.0f;
	const auto defense_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto economy_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 0.0f;
	const auto unit_morale_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto research_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto mineral_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 0.0f;
	const auto psych_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 0.0f;
	const auto population_limit = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto required_facility = buf.GetRemaining() > 0 ? buf.ReadString() : "";
	const auto drone_modifier = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto talent_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto suppress_psych = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto unit_morale_land_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto unit_morale_water_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto unit_morale_air_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto water_defense_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto air_defense_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto growth_rating_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto native_lifecycle_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto is_project = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto granted_facility = buf.GetRemaining() > 0 ? buf.ReadString() : "";
	const auto global_talent_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_growth_rating_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_population_limit_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_mineral_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_support_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_maintenance_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto global_native_lifecycle_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto network_node_drone_modifier = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto network_node_research_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto worked_tile_energy_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_prevent_riots = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto global_terraforming_rate_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto new_base_population = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto small_base_drone_modifier = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_psi_attack_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto global_psi_defense_multiplier = buf.GetRemaining() > 0 ? buf.ReadFloat() : 1.0f;
	const auto global_naval_movement_bonus = buf.GetRemaining() > 0 ? buf.ReadFloat() : 0.0f;
	const auto global_full_repair = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto required_project = buf.GetRemaining() > 0 ? buf.ReadString() : "";
	const auto forest_nutrient_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto forest_mineral_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto forest_energy_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto full_repair_land = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto full_repair_water = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto full_repair_air = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto full_repair_native = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto defender_morale_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_police_rating_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto global_extra_police_units = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto efficiency_rating_bonus = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto defender_morale_minimum = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto prototype_cost_waiver = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto mineral_to_energy_divisor = buf.GetRemaining() > 0 ? buf.ReadInt() : 0;
	const auto orbital_resource = buf.GetRemaining() > 0 ? buf.ReadString() : "";
	const auto orbital_defense = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	const auto psi_gate = buf.GetRemaining() > 0 ? buf.ReadBool() : false;
	return new FacilityDef(
		id,
		name,
		mineral_cost,
		nutrient_bonus,
		mineral_bonus,
		energy_bonus,
		energy_maintenance,
		required_technology,
		psych_bonus,
		research_multiplier,
		defense_multiplier,
		economy_multiplier,
		unit_morale_bonus,
		research_bonus,
		mineral_multiplier,
		psych_multiplier,
		population_limit,
		required_facility,
		drone_modifier,
		talent_bonus,
		suppress_psych,
		unit_morale_land_bonus,
		unit_morale_water_bonus,
		unit_morale_air_bonus,
		water_defense_multiplier,
		air_defense_multiplier,
		growth_rating_bonus,
		native_lifecycle_bonus,
		is_project,
		granted_facility,
		global_talent_bonus,
		global_growth_rating_bonus,
		global_population_limit_bonus,
		global_mineral_bonus,
		global_support_bonus,
		global_maintenance_multiplier,
		global_native_lifecycle_bonus,
		network_node_drone_modifier,
		network_node_research_bonus,
		worked_tile_energy_bonus,
		global_prevent_riots,
		global_terraforming_rate_multiplier,
		new_base_population,
		small_base_drone_modifier,
		global_psi_attack_multiplier,
		global_psi_defense_multiplier,
		global_naval_movement_bonus,
		global_full_repair,
		required_project,
		forest_nutrient_bonus,
		forest_mineral_bonus,
		forest_energy_bonus,
		full_repair_land,
		full_repair_water,
		full_repair_air,
		full_repair_native,
		defender_morale_bonus,
		global_police_rating_bonus,
		global_extra_police_units,
		efficiency_rating_bonus,
		defender_morale_minimum,
		prototype_cost_waiver,
		mineral_to_energy_divisor,
		orbital_resource,
		orbital_defense,
		psi_gate
	);
}

WRAPIMPL_BEGIN( FacilityDef )
	WRAPIMPL_PROPS
		{
			"id",
			VALUE( gse::value::String, , m_id )
		},
		{
			"name",
			VALUE( gse::value::String, , m_name )
		},
		{
			"production_kind",
			VALUE( gse::value::String, , m_is_project ? "project" : "facility" )
		},
		{
			"mineral_cost",
			VALUE( gse::value::Int, , m_mineral_cost )
		},
		{
			"nutrient_bonus",
			VALUE( gse::value::Int, , m_nutrient_bonus )
		},
		{
			"mineral_bonus",
			VALUE( gse::value::Int, , m_mineral_bonus )
		},
		{
			"energy_bonus",
			VALUE( gse::value::Int, , m_energy_bonus )
		},
		{
			"energy_maintenance",
			VALUE( gse::value::Int, , m_energy_maintenance )
		},
		{
			"required_technology",
			VALUE( gse::value::String, , m_required_technology )
		},
		{
			"psych_bonus",
			VALUE( gse::value::Int, , m_psych_bonus )
		},
		{
			"research_multiplier",
			VALUE( gse::value::Float, , m_research_multiplier )
		},
		{
			"defense_multiplier",
			VALUE( gse::value::Float, , m_defense_multiplier )
		},
		{
			"economy_multiplier",
			VALUE( gse::value::Float, , m_economy_multiplier )
		},
		{
			"unit_morale_bonus",
			VALUE( gse::value::Int, , m_unit_morale_bonus )
		},
		{
			"research_bonus",
			VALUE( gse::value::Int, , m_research_bonus )
		},
		{
			"mineral_multiplier",
			VALUE( gse::value::Float, , m_mineral_multiplier )
		},
		{
			"psych_multiplier",
			VALUE( gse::value::Float, , m_psych_multiplier )
		},
		{
			"population_limit",
			VALUE( gse::value::Int, , m_population_limit )
		},
		{
			"required_facility",
			VALUE( gse::value::String, , m_required_facility )
		},
		{
			"required_project",
			VALUE( gse::value::String, , m_required_project )
		},
		{
			"drone_modifier",
			VALUE( gse::value::Int, , m_drone_modifier )
		},
		{
			"talent_bonus",
			VALUE( gse::value::Int, , m_talent_bonus )
		},
		{
			"suppress_psych",
			VALUE( gse::value::Bool, , m_suppress_psych )
		},
		{
			"unit_morale_land_bonus",
			VALUE( gse::value::Int, , m_unit_morale_land_bonus )
		},
		{
			"unit_morale_water_bonus",
			VALUE( gse::value::Int, , m_unit_morale_water_bonus )
		},
		{
			"unit_morale_air_bonus",
			VALUE( gse::value::Int, , m_unit_morale_air_bonus )
		},
		{
			"water_defense_multiplier",
			VALUE( gse::value::Float, , m_water_defense_multiplier )
		},
		{
			"air_defense_multiplier",
			VALUE( gse::value::Float, , m_air_defense_multiplier )
		},
		{
			"growth_rating_bonus",
			VALUE( gse::value::Int, , m_growth_rating_bonus )
		},
		{
			"native_lifecycle_bonus",
			VALUE( gse::value::Int, , m_native_lifecycle_bonus )
		},
		{
			"is_project",
			VALUE( gse::value::Bool, , m_is_project )
		},
		{
			"granted_facility",
			VALUE( gse::value::String, , m_granted_facility )
		},
		{
			"global_talent_bonus",
			VALUE( gse::value::Int, , m_global_talent_bonus )
		},
		{
			"global_growth_rating_bonus",
			VALUE( gse::value::Int, , m_global_growth_rating_bonus )
		},
		{
			"global_population_limit_bonus",
			VALUE( gse::value::Int, , m_global_population_limit_bonus )
		},
		{
			"global_mineral_bonus",
			VALUE( gse::value::Int, , m_global_mineral_bonus )
		},
		{
			"global_support_bonus",
			VALUE( gse::value::Int, , m_global_support_bonus )
		},
		{
			"global_maintenance_multiplier",
			VALUE( gse::value::Float, , m_global_maintenance_multiplier )
		},
		{
			"global_native_lifecycle_bonus",
			VALUE( gse::value::Int, , m_global_native_lifecycle_bonus )
		},
		{
			"network_node_drone_modifier",
			VALUE( gse::value::Int, , m_network_node_drone_modifier )
		},
		{
			"network_node_research_bonus",
			VALUE( gse::value::Int, , m_network_node_research_bonus )
		},
		{
			"worked_tile_energy_bonus",
			VALUE( gse::value::Int, , m_worked_tile_energy_bonus )
		},
		{
			"global_prevent_riots",
			VALUE( gse::value::Bool, , m_global_prevent_riots )
		},
		{
			"global_terraforming_rate_multiplier",
			VALUE( gse::value::Float, , m_global_terraforming_rate_multiplier )
		},
		{
			"new_base_population",
			VALUE( gse::value::Int, , m_new_base_population )
		},
		{
			"small_base_drone_modifier",
			VALUE( gse::value::Int, , m_small_base_drone_modifier )
		},
		{
			"global_psi_attack_multiplier",
			VALUE( gse::value::Float, , m_global_psi_attack_multiplier )
		},
		{
			"global_psi_defense_multiplier",
			VALUE( gse::value::Float, , m_global_psi_defense_multiplier )
		},
		{
			"global_naval_movement_bonus",
			VALUE( gse::value::Float, , m_global_naval_movement_bonus )
		},
		{
			"global_full_repair",
			VALUE( gse::value::Bool, , m_global_full_repair )
		},
		{
			"forest_nutrient_bonus",
			VALUE( gse::value::Int, , m_forest_nutrient_bonus )
		},
		{
			"forest_mineral_bonus",
			VALUE( gse::value::Int, , m_forest_mineral_bonus )
		},
		{
			"forest_energy_bonus",
			VALUE( gse::value::Int, , m_forest_energy_bonus )
		},
		{
			"full_repair_land",
			VALUE( gse::value::Bool, , m_full_repair_land )
		},
		{
			"full_repair_water",
			VALUE( gse::value::Bool, , m_full_repair_water )
		},
		{
			"full_repair_air",
			VALUE( gse::value::Bool, , m_full_repair_air )
		},
		{
			"full_repair_native",
			VALUE( gse::value::Bool, , m_full_repair_native )
		},
		{
			"defender_morale_bonus",
			VALUE( gse::value::Int, , m_defender_morale_bonus )
		},
		{
			"global_police_rating_bonus",
			VALUE( gse::value::Int, , m_global_police_rating_bonus )
		},
		{
			"global_extra_police_units",
			VALUE( gse::value::Int, , m_global_extra_police_units )
		},
		{
			"efficiency_rating_bonus",
			VALUE( gse::value::Int, , m_efficiency_rating_bonus )
		},
		{
			"defender_morale_minimum",
			VALUE( gse::value::Int, , m_defender_morale_minimum )
		},
		{
			"prototype_cost_waiver",
			VALUE( gse::value::Bool, , m_prototype_cost_waiver )
		},
		{
			"mineral_to_energy_divisor",
			VALUE( gse::value::Int, , m_mineral_to_energy_divisor )
		},
		{
			"orbital_resource",
			VALUE( gse::value::String, , m_orbital_resource )
		},
		{
			"orbital_defense",
			VALUE( gse::value::Bool, , m_orbital_defense )
		},
		{
			"psi_gate",
			VALUE( gse::value::Bool, , m_psi_gate )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( FacilityDef )

}
}
}
