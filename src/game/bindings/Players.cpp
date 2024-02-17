#include "Binding.h"

#include "game/State.h"

#include "gse/type/Array.h"
#include "gse/type/String.h"

namespace game {
namespace bindings {

BINDING_IMPL( players ) {
	const gse::type::Object::properties_t properties = {
		{
			"get_all",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );

				const auto& slots = m_bindings->GetState()->m_slots.GetSlots();
				gse::type::Array::elements_t elements = {};
				for ( const auto& slot : slots ) {
					const auto state = slot.GetState();
					if ( state == Slot::SS_OPEN || state == Slot::SS_CLOSED ) {
						continue; // skip
					}
					elements.push_back( slot.Wrap() );
				}
				return VALUE( gse::type::Array, elements );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
