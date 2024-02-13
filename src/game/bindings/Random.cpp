#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/Int.h"

namespace game {
namespace bindings {

BINDING_IMPL( random ) {
	const gse::type::Object::properties_t properties = {
		{
			"get_int",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( min, 0, Int );
				N_GETVALUE( max, 1, Int );
				if ( max < min ) {
					ERROR( gse::EC.INVALID_CALL, "Maximum value is smaller than minimum ( " + std::to_string( max ) + " < " + std::to_string( min ) + " )" );
				}
				return VALUE( gse::type::Int, m_game->GetRandom()->GetInt64( min, max ) );
			})
		}
	};
	return VALUE( gse::type::Object, properties );
}

}
}
