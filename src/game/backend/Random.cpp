#include "Random.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"

namespace game {
namespace backend {

Random::Random( Game* game, const util::random::value_t seed )
	: util::random::Random( seed )
	, m_game( game ) {
	//
}

WRAPIMPL_BEGIN( Random )
	WRAPIMPL_PROPS
		{
			"get_int",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( min, 0, Int );
				N_GETVALUE( max, 1, Int );
				if ( max < min ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Maximum value is smaller than minimum ( " + std::to_string( max ) + " < " + std::to_string( min ) + " )" );
				}
				if ( !m_game->GetState()->IsMaster() ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Only master is allowed to generate random values" );
				}
				return VALUE( gse::type::Int, m_game->GetRandom()->GetInt64( min, max ) );
			})
		},
		{
			"get_float",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( min, 0, Float );
				N_GETVALUE( max, 1, Float );
				if ( max < min ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Maximum value is smaller than minimum ( " + std::to_string( max ) + " < " + std::to_string( min ) + " )" );
				}
				if ( !m_game->GetState()->IsMaster() ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Only master is allowed to generate random values" );
				}
				return VALUE( gse::type::Float, m_game->GetRandom()->GetFloat( min, max ) );
			})
		},
	};
WRAPIMPL_END_PTR( Random )

UNWRAPIMPL_PTR( Random )

}
}
