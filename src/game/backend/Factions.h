#pragma once

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

namespace game {
namespace backend {

class Faction;

class Factions : public gse::Wrappable {
public:
	Factions();
	~Factions();

	void Add( Faction* faction );
	void Remove( const std::string& id );
	void Clear();
	Faction* Get( const std::string& id ) const;
	const std::vector< Faction* > GetAll() const;

	WRAPDEFS_PTR( Factions )

	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );

private:
	struct faction_t {
		Faction* faction = {};
		size_t index = 0;
	};
	typedef std::unordered_map< std::string, faction_t > factions_t;
	factions_t m_factions = {};
	std::map< size_t, std::string > m_factions_order = {};
	size_t m_next_faction_idx = 0;

};

}
}
