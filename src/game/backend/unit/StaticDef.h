#pragma once

#include "Def.h"

#include "Types.h"

namespace game {
namespace backend {
namespace unit {

class Render;

class StaticDef : public Def {
public:
	static const std::string& GetMovementTypeString( const movement_type_t movement_type );

	static const health_t HEALTH_MAX;
	static const health_t HEALTH_PER_TURN;

	StaticDef(
		const std::string& id,
		const MoraleSet* moraleset,
		const std::string& name,
		const movement_type_t movement_type,
		const movement_t movement_per_turn,
		const Render* render
	);
	~StaticDef();

	const movement_type_t m_movement_type;
	const movement_t m_movement_per_turn;
	const Render* m_render;

	const movement_type_t GetMovementType() const override;

	const std::string ToString( const std::string& prefix ) const override;

	WRAPDEFS_PTR( StaticDef );

private:
	static const std::unordered_map< movement_type_t, std::string > s_movement_type_str;

private:
	friend class Def;

	static void Serialize( types::Buffer& buf, const StaticDef* def );
	static StaticDef* Deserialize( types::Buffer& buf, const std::string& id, const std::string& moraleset_name, const std::string& name );

};

}
}
}
