#pragma once

#include <cstdint>
#include <string>

#include "gse/Wrappable.h"
#include "types/Buffer.h"

namespace game {
namespace backend {
namespace base {

class FacilityDef : public gse::Wrappable {
public:

	static constexpr int64_t MAX_MINERAL_COST = 1000000;
	static constexpr int64_t MAX_RESOURCE_BONUS = 1000000;
	static constexpr int64_t MAX_ENERGY_MAINTENANCE = 1000000;
	static constexpr float MAX_RESEARCH_MULTIPLIER = 10.0f;
	static constexpr float MIN_RESEARCH_MULTIPLIER = -1.0f;
	static constexpr float MAX_DEFENSE_MULTIPLIER = 10.0f;
	static constexpr float MAX_ECONOMY_MULTIPLIER = 10.0f;
	static constexpr float MAX_MINERAL_MULTIPLIER = 10.0f;
	static constexpr float MAX_PSYCH_MULTIPLIER = 10.0f;
	static constexpr int64_t MAX_UNIT_MORALE_BONUS = 10;
	static constexpr int64_t MAX_POPULATION_LIMIT = 1000000;
	static constexpr int64_t MAX_DRONE_MODIFIER = 1000000;
	static constexpr int64_t MAX_GROWTH_RATING_BONUS = 10;

	FacilityDef(
		const std::string& id,
		const std::string& name,
		const int64_t mineral_cost,
		const int64_t nutrient_bonus,
		const int64_t mineral_bonus,
		const int64_t energy_bonus,
		const int64_t energy_maintenance,
		const std::string& required_technology = "",
		const int64_t psych_bonus = 0,
		const float research_multiplier = 0.0f,
		const float defense_multiplier = 1.0f,
		const float economy_multiplier = 0.0f,
		const int64_t unit_morale_bonus = 0,
		const int64_t research_bonus = 0,
		const float mineral_multiplier = 0.0f,
		const float psych_multiplier = 0.0f,
		const int64_t population_limit = 0,
		const std::string& required_facility = "",
		const int64_t drone_modifier = 0,
		const int64_t talent_bonus = 0,
		const bool suppress_psych = false,
		const int64_t unit_morale_land_bonus = 0,
		const int64_t unit_morale_water_bonus = 0,
		const int64_t unit_morale_air_bonus = 0,
		const float water_defense_multiplier = 1.0f,
		const float air_defense_multiplier = 1.0f,
		const int64_t growth_rating_bonus = 0,
		const int64_t native_lifecycle_bonus = 0,
		const bool is_project = false,
		const std::string& granted_facility = "",
		const int64_t global_talent_bonus = 0,
		const int64_t global_growth_rating_bonus = 0,
		const int64_t global_population_limit_bonus = 0,
		const int64_t global_mineral_bonus = 0,
		const int64_t global_support_bonus = 0,
		const float global_maintenance_multiplier = 1.0f,
		const int64_t global_native_lifecycle_bonus = 0,
		const int64_t network_node_drone_modifier = 0,
		const int64_t network_node_research_bonus = 0,
		const int64_t worked_tile_energy_bonus = 0,
		const bool global_prevent_riots = false,
		const float global_terraforming_rate_multiplier = 1.0f,
		const int64_t new_base_population = 0,
		const int64_t small_base_drone_modifier = 0,
		const float global_psi_attack_multiplier = 1.0f,
		const float global_psi_defense_multiplier = 1.0f,
		const float global_naval_movement_bonus = 0.0f,
		const bool global_full_repair = false,
		const std::string& required_project = "",
		const int64_t forest_nutrient_bonus = 0,
		const int64_t forest_mineral_bonus = 0,
		const int64_t forest_energy_bonus = 0,
		const bool full_repair_land = false,
		const bool full_repair_water = false,
		const bool full_repair_air = false,
		const bool full_repair_native = false,
		const int64_t defender_morale_bonus = 0,
		const int64_t global_police_rating_bonus = 0,
		const int64_t global_extra_police_units = 0,
		const int64_t efficiency_rating_bonus = 0,
		const int64_t defender_morale_minimum = 0,
		const bool prototype_cost_waiver = false,
		const int64_t mineral_to_energy_divisor = 0,
		const std::string& orbital_resource = "",
		const bool orbital_defense = false,
		const bool psi_gate = false
	);
	virtual ~FacilityDef() = default;

	const std::string m_id;
	const std::string m_name;
	const int64_t m_mineral_cost;
	const int64_t m_nutrient_bonus;
	const int64_t m_mineral_bonus;
	const int64_t m_energy_bonus;
	const int64_t m_energy_maintenance;
	const std::string m_required_technology;
	const int64_t m_psych_bonus;
	const float m_research_multiplier;
	const float m_defense_multiplier;
	const float m_economy_multiplier;
	const int64_t m_unit_morale_bonus;
	const int64_t m_research_bonus;
	const float m_mineral_multiplier;
	const float m_psych_multiplier;
	const int64_t m_population_limit;
	const std::string m_required_facility;
	const int64_t m_drone_modifier;
	const int64_t m_talent_bonus;
	const bool m_suppress_psych;
	const int64_t m_unit_morale_land_bonus;
	const int64_t m_unit_morale_water_bonus;
	const int64_t m_unit_morale_air_bonus;
	const float m_water_defense_multiplier;
	const float m_air_defense_multiplier;
	const int64_t m_growth_rating_bonus;
	const int64_t m_native_lifecycle_bonus;
	const bool m_is_project;
	const std::string m_granted_facility;
	const int64_t m_global_talent_bonus;
	const int64_t m_global_growth_rating_bonus;
	const int64_t m_global_population_limit_bonus;
	const int64_t m_global_mineral_bonus;
	const int64_t m_global_support_bonus;
	const float m_global_maintenance_multiplier;
	const int64_t m_global_native_lifecycle_bonus;
	const int64_t m_network_node_drone_modifier;
	const int64_t m_network_node_research_bonus;
	const int64_t m_worked_tile_energy_bonus;
	const bool m_global_prevent_riots;
	const float m_global_terraforming_rate_multiplier;
	const int64_t m_new_base_population;
	const int64_t m_small_base_drone_modifier;
	const float m_global_psi_attack_multiplier;
	const float m_global_psi_defense_multiplier;
	const float m_global_naval_movement_bonus;
	const bool m_global_full_repair;
	const std::string m_required_project;
	const int64_t m_forest_nutrient_bonus;
	const int64_t m_forest_mineral_bonus;
	const int64_t m_forest_energy_bonus;
	const bool m_full_repair_land;
	const bool m_full_repair_water;
	const bool m_full_repair_air;
	const bool m_full_repair_native;
	const int64_t m_defender_morale_bonus;
	const int64_t m_global_police_rating_bonus;
	const int64_t m_global_extra_police_units;
	const int64_t m_efficiency_rating_bonus;
	const int64_t m_defender_morale_minimum;
	const bool m_prototype_cost_waiver;
	const int64_t m_mineral_to_energy_divisor;
	const std::string m_orbital_resource;
	const bool m_orbital_defense;
	const bool m_psi_gate;

	static const types::Buffer Serialize( const FacilityDef* def );
	static FacilityDef* Deserialize( types::Buffer& buf );

	WRAPDEFS_PTR( FacilityDef );

};

}
}
}
