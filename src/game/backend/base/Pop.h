#pragma once

#include "types/Buffer.h"

#include "gse/Wrappable.h"

namespace game {
namespace backend {

class Game;

namespace map::tile {
class Tile;
}

namespace base {

class Base;
class PopDef;

class Pop : public gse::Wrappable {
public:

	Pop( Base* base = nullptr, const size_t id = 0, const PopDef* def = nullptr, const uint8_t variant = 0, map::tile::Tile* const worked_tile = nullptr );

	void Serialize( types::Buffer& buf ) const;
	void Deserialize( types::Buffer& buf, Game* game );

	void SetBase( Base* const base );
	bool HasWorkedTileLink() const;
	map::tile::Tile* GetWorkedTileLink() const;
	void SetWorkedTile( GSE_CALLABLE, map::tile::Tile* const tile );
	void UnsetWorkedTile( GSE_CALLABLE, const map::tile::Tile* const tile );

	WRAPDEFS_PTR( Pop );
	WRAPDEF_SERIALIZABLE;

	Base* m_base;
	size_t m_id;
	const PopDef* m_def;
	uint8_t m_variant = 0;
	map::tile::Tile* m_worked_tile = nullptr;

};

}
}
}
