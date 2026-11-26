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

	Def( const std::string& id, const MoraleSet* moraleset, const def_type_t type, const std::string& name, const size_t cost );
	virtual ~Def() = default;

	const std::string m_id;
	const MoraleSet* m_moraleset;
	const def_type_t m_type;
	const std::string m_name;
	const size_t m_cost;

	virtual const movement_type_t GetMovementType() const = 0;

	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	static const types::Buffer Serialize( const Def* def );
	static Def* Deserialize( types::Buffer& buf );

	WRAPDEFS_PTR( Def );

	virtual const std::string GetRenderTextureString() const = 0;

};

}
}
}
