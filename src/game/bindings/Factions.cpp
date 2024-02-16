#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/String.h"

#include "game/State.h"

#include "types/Color.h"

namespace game {
namespace bindings {

BINDING_IMPL( factions ) {
	auto& factions = m_bindings->GetState()->m_settings.global.game_rules.m_factions;
	const gse::type::Object::properties_t properties = {
		{
			"define",
			NATIVE_CALL( this, &factions ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( faction_def, 1, Object );
				N_GETPROP( name, faction_def, "name", String );
				N_GETPROP_UNWRAP( color, faction_def, "color", types::Color );
				if ( factions.find( id ) != factions.end() ) {
					ERROR( gse::EC.GAME_ERROR, "Faction '" + id + "' already exists" );
				}
				const auto* faction = new rules::Faction( id, name, color );
				factions.insert({ id, rules::Faction{ id, name, color } });
				return VALUE( gse::type::Undefined );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
