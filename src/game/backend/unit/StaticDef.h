#pragma once

#include <set>

#include "Def.h"

#include "Types.h"

namespace game {
namespace backend {
namespace unit {

class Render;

class StaticDef : public Def {
public:
	static constexpr size_t MAX_ABILITIES = 64;
	static constexpr int64_t MAX_OPERATIONAL_RANGE = 1000;
	static constexpr int64_t MAX_CARGO_CAPACITY = 100;
	static const std::string& GetMovementTypeString( const movement_type_t movement_type );

	static const health_t HEALTH_MAX;
	static const health_t HEALTH_PER_TURN;

	StaticDef(
		const std::string& id,
		const MoraleSet* moraleset,
		const std::string& name,
		const int64_t mineral_cost,
		const std::string& required_technology,
		const bool is_native,
		const int64_t offense,
		const int64_t defense,
		const bool can_found_base,
		const bool can_terraform,
		const movement_type_t movement_type,
		const movement_t movement_per_turn,
		const Render* render,
		const std::string& chassis_id = "",
		const std::string& weapon_id = "",
		const std::string& armor_id = "",
		const std::string& reactor_id = "",
		const int64_t reactor_power = 1,
		const std::set< std::string >& abilities = {},
		const int64_t operational_range = 0,
		const bool is_missile = false,
		const int64_t cargo_capacity = 0,
		const bool buildable = true
	);
	~StaticDef();

	const movement_type_t m_movement_type;
	const movement_t m_movement_per_turn;
	const Render* m_render;
	const std::string m_chassis_id;
	const std::string m_weapon_id;
	const std::string m_armor_id;
	const std::string m_reactor_id;
	const int64_t m_reactor_power;
	const std::set< std::string > m_abilities;
	const int64_t m_operational_range;
	const bool m_is_missile;
	const int64_t m_cargo_capacity;

	const bool HasAbility( const std::string& id ) const;
	const bool IsArtillery() const;
	const bool IsPsiAttack() const;
	const bool IsPsiDefense() const;

	const movement_type_t GetMovementType() const override;

	const std::string ToString( const std::string& prefix ) const override;

	WRAPDEFS_PTR( StaticDef );

private:
	static const std::unordered_map< movement_type_t, std::string > s_movement_type_str;

private:
	friend class Def;

	static void Serialize( types::Buffer& buf, const StaticDef* def );
	static StaticDef* Deserialize(
		types::Buffer& buf,
		const std::string& id,
		const std::string& moraleset_name,
		const std::string& name,
		const int64_t mineral_cost,
		const std::string& required_technology,
		const bool is_native,
		const int64_t offense,
		const int64_t defense,
		const bool can_found_base,
		const bool can_terraform
	);

};

}
}
}
