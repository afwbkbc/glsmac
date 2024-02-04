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
	NEW( m_gse, gse::GSE );
	m_gse->AddBindings( this );
	m_gse_context = m_gse->CreateGlobalContext();
	m_gse_context->IncRefs();
}

Bindings::~Bindings() {
	m_gse_context->DecRefs();
	DELETE( m_gse );
}

void Bindings::AddToContext( gse::Context* ctx ) {

#define CALLBACK( _type ) NATIVE_CALL( this ) { \
		N_EXPECT_ARGS( 1 ); \
		N_GET( cb, 0 ); \
		N_CHECKTYPE( cb.Get(), 0, Callable ); \
		SetCallback( _type, cb, ctx, call_si ); \
		return VALUE( type::Undefined ); \
	})

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
			"get_player",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				return GetPlayer( m_game->GetPlayer() );
			})
		},
		{
			"on_start", CALLBACK( CS_ONSTART )
		},
		{
			"on_turn", CALLBACK( CS_ONTURN )
		},
	};
	ctx->CreateConst( "game", VALUE( type::Object, methods ), nullptr );
}

void Bindings::RunMain() {
	m_gse->GetInclude( m_gse_context, m_si_internal, m_entry_script );
}

void Bindings::Call( const callback_slot_t slot, const callback_arguments_t* arguments ) {
	const auto& it = m_callbacks.find( slot );
	if ( it != m_callbacks.end() ) {
		((gse::type::Callable*)it->second.Get())->Run( m_gse_context, m_si_internal, arguments ? *arguments : m_no_arguments );
	}
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
		},
	};
	return VALUE( type::Object, properties );
}

/*const gse::Value Bindings::GetTurn( const Turn* turn ) const {
	const type::Object::properties_t properties = {
		{
			"year", VALUE( type::Int, turn->GetYear() ),
		},
	};
	return VALUE( type::Object, properties );
}*/

void Bindings::SetCallback( const callback_slot_t slot, const gse::Value& callback, const gse::Context* context, const si_t& si ) {
	if ( m_callbacks.find( slot ) != m_callbacks.end() ) {
		throw gse::Exception( EC.GAME_API_ERROR, "Callback slot already in use", context, si );
	}
	m_callbacks.insert_or_assign( slot, callback );
}

}
