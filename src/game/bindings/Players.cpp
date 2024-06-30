#include "Binding.h"

#include "game/State.h"
#include "game/slot/Slot.h"
#include "game/slot/Slots.h"
#include "game/bindings/Bindings.h"
#include "gse/callable/Native.h"
#include "gse/type/Array.h"
#include "gse/type/String.h"
#include "gse/type/Undefined.h"

namespace game {
namespace bindings {

BINDING_IMPL( players ) {
	const gse::type::object_properties_t properties = {
		{
			"get_all",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );

				auto& slots = m_bindings->GetState()->m_slots->GetSlots();
				gse::type::array_elements_t elements = {};
				for ( auto& slot : slots ) {
					const auto state = slot.GetState();
					if ( state == slot::Slot::SS_OPEN || state == slot::Slot::SS_CLOSED ) {
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
