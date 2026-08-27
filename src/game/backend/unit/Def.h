#pragma once

#include <string>

#include "Types.h"
#include "types/Buffer.h"

#include "game/backend/map/tile/Types.h"
#include "gse/Wrappable.h"

namespace game {
namespace backend {

namespace map {
namespace tile {
class Tile;
class TileState;
}
}

namespace unit {

class MoraleSet;

class Def : public gse::Wrappable {
public:

	static constexpr int64_t MAX_MINERAL_COST = 1000000;
	static constexpr int64_t MAX_COMBAT_STRENGTH = 1000000;

	Def(
		const std::string& id,
		const MoraleSet* moraleset,
		const def_type_t type,
		const std::string& name,
		const int64_t mineral_cost,
		const std::string& required_technology,
		const bool is_native,
		const int64_t offense,
		const int64_t defense,
		const bool can_found_base,
		const bool can_terraform,
		const bool buildable
	);
	virtual ~Def() = default;

	const std::string m_id;
	const MoraleSet* m_moraleset;
	const def_type_t m_type;
	const std::string m_name;
	const int64_t m_mineral_cost;
	const std::string m_required_technology;
	const bool m_is_native;
	const int64_t m_offense;
	const int64_t m_defense;
	const bool m_can_found_base;
	const bool m_can_terraform;
	const bool m_buildable;

	virtual const movement_type_t GetMovementType() const = 0;

	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	static const types::Buffer Serialize( const Def* def );
	static Def* Deserialize( types::Buffer& buf );

	WRAPDEFS_PTR( Def );

};

}
}
}
