#include "Bindings.h"

#include "game/Game.h"

#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Object.h"
#include "gse/type/Callable.h"
#include "gse/callable/Native.h"
#include "gse/type/Undefined.h"

using namespace gse;

namespace game {

Bindings::Bindings( Game* game )
	: m_game( game ) {
}

void Bindings::AddToContext( gse::Context* ctx ) const {
	type::Object::properties_t methods = {
		{
			"message",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( text, 0, String );
				m_game->Message( text );
				return VALUE( type::Undefined );
			})
		},
		{
			"exit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				m_game->Quit();
				return VALUE( type::Undefined );
			})
		},
		{
			"player",
			GetPlayer( m_game->GetPlayer() )
		}
	};
	ctx->CreateVariable( "game", VALUE( type::Object, methods ), nullptr );
}

const Value Bindings::GetPlayer( const Player* player ) const {
	const type::Object::properties_t properties = {
		{
			{
				"name",VALUE( type::String, player->GetPlayerName() ),
			},
			{
				"full_name", VALUE( type::String, player->GetFullName() ),
			},
			{
				"faction", GetFaction( &player->GetFaction() ),
			},
			{
				"difficulty",GetDifficulty( &player->GetDifficultyLevel() )
			},
		}
	};
	return VALUE( type::Object, properties );
}

const gse::Value Bindings::GetFaction( const rules::Faction* faction ) const {
	const type::Object::properties_t properties = {
		{
			"name", VALUE( type::String, faction->m_name ),
		}
	};
	return VALUE( type::Object, properties );
}

const gse::Value Bindings::GetDifficulty( const rules::DifficultyLevel* difficulty ) const {
	const type::Object::properties_t properties = {
		{
			"name",VALUE( type::String, difficulty->m_name ),
		},
		{
			"value", VALUE( type::Int, difficulty->m_difficulty ),
		}
	};
	return VALUE( type::Object, properties );
}

}
