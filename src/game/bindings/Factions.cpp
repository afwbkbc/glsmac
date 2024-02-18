#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/String.h"

#include "game/State.h"

#include "types/Color.h"

namespace game {
namespace bindings {

BINDING_IMPL( factions ) {
	auto& factions = m_bindings->GetState()->m_settings.global.game_rules.m_factions;
	auto& factions_order = m_bindings->GetState()->m_settings.global.game_rules.m_factions_order;
	const gse::type::Object::properties_t properties = {
		{
			"define",
			NATIVE_CALL( this, &factions, &factions_order ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				if ( factions.find( id ) != factions.end() ) {
					ERROR( gse::EC.GAME_ERROR, "Faction '" + id + "' already exists" );
				}
				N_GETVALUE( faction_def, 1, Object );
				N_GETPROP( name, faction_def, "name", String );

				rules::Faction faction = { id, name };

				N_GETPROP( files, faction_def, "files", Object );
				N_GETPROP( pcx_file, files, "pcx", String );
				faction.ImportPCX( pcx_file );

				N_GETPROP_OPTBOOL( is_progenitor, faction_def, "is_progenitor")
				if ( is_progenitor ) {
					faction.m_flags |= rules::Faction::FF_PROGENITOR;
				}
				factions.insert({ id, faction });
				factions_order.push_back( id );
				return VALUE( gse::type::Undefined );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
