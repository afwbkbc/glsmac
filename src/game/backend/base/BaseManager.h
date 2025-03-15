#pragma once

#include "common/Common.h"

#include "gse/GCWrappable.h"
#include "gse/value/Object.h"

#include "Types.h"

namespace game {
namespace backend {

class Game;

namespace map {
class Map;
namespace tile {
class Tile;
}
}

namespace slot {
class Slot;
}

namespace base {

class Base;
class PopDef;

CLASS2( BaseManager, common::Class, gse::GCWrappable )
public:
	BaseManager( Game* game );
	~BaseManager();

	void Clear();

	PopDef* GetPopDef( const std::string& id ) const;
	Base* GetBase( const size_t id ) const;
	void DefinePop( base::PopDef* pop_def );
	void SpawnBase( GSE_CALLABLE, base::Base* base );

	const std::map< size_t, Base* >& GetBases() const;

	void ProcessUnprocessed( GSE_CALLABLE );
	void PushUpdates();

	WRAPDEFS_PTR( BaseManager )

	void Serialize( types::Buffer& buf ) const;
	void Unserialize( GSE_CALLABLE, types::Buffer& buf );

	void RefreshBase( const base::Base* base );

private:
	Game* m_game = nullptr;

	std::unordered_map< std::string, base::PopDef* > m_base_popdefs = {};
	std::map< size_t, base::Base* > m_bases = {};
	std::vector< base::Base* > m_unprocessed_bases = {};

	std::unordered_set< std::string > m_registered_base_names = {};

	enum base_update_op_t : uint8_t {
		BUO_NONE = 0,
		BUO_SPAWN = 1 << 0,
		BUO_REFRESH = 1 << 1,
		BUO_DESPAWN = 1 << 2,
	};
	struct base_update_t {
		base_update_op_t ops = BUO_NONE;
		const base::Base* base = nullptr;
	};
	std::unordered_map< size_t, base_update_t > m_base_updates = {};

	void QueueBaseUpdate( const base::Base* base, const base_update_op_t op );

private:
	friend class Base;
};

}
}
}
