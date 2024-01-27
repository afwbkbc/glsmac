#include "Bindings.h"

#include "game/Game.h"

#include "gse/type/String.h"
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
	};
	ctx->CreateVariable( "game", VALUE( type::Object, methods ), nullptr );
}

}
