#include "Binding.h"

#include "game/backend/Game.h"
#include "Bindings.h"

#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"

#include "game/backend/Resource.h"
#include "game/backend/event/DefineResource.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( resources ) {
	const gse::type::object_properties_t properties = {
		{
			"define",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( def, 1, Object );
				N_GETPROP( name, def, "name", String );
				N_GETPROP( render, def, "render", Object );
				N_GETPROP( type, render, "type", String );

				if ( type == "sprite_map" ) {
					N_GETPROP( file, render, "file", String );

					N_GETPROP( yields, render, "yields", Object );
					N_GETPROP( grid_x, yields, "grid_x", Int );
					N_GETPROP( grid_y, yields, "grid_y", Int );
					N_GETPROP( grid_margin, yields, "grid_margin", Int );
					N_GETPROP( cell_width, yields, "cell_width", Int );
					N_GETPROP( cell_height, yields, "cell_height", Int );
					N_GETPROP( cells_count, yields, "cells_count", Int );

#define X( _n ) \
					N_GETPROP( _n, render, "plus", Object ); \
					N_GETPROP( _n ## _x, _n, "x", Int ); \
					N_GETPROP( _n ## _y, _n, "y", Int ); \
					N_GETPROP( _n ## _width, _n, "width", Int ); \
					N_GETPROP( _n ## _height, _n, "height", Int );
					X( plus )
					X( minus )

					auto* resource = new Resource(
						id,
						name,
						{
							file,
							{
								(uint16_t)grid_x, (uint16_t)grid_y, (uint8_t)grid_margin,
								(uint16_t)cell_width, (uint16_t)cell_height, (uint8_t)cells_count,
							},
							{
								(uint16_t)plus_x, (uint16_t)plus_y,
								(uint16_t)plus_width, (uint16_t)plus_height,
							},
							{
								(uint16_t)minus_x, (uint16_t)minus_y,
								(uint16_t)minus_width, (uint16_t)minus_height,
							},
						}
					);
					auto* game = GAME;
					return game->AddEvent( new event::DefineResource( game->GetSlotNum(), resource ) );
				}
				else {
					ERROR( gse::EC.GAME_ERROR, "Unsupported resource type: " + type );
				}

				return VALUE( gse::type::Undefined );
			} )
		},
	};
	return VALUE( gse::type::Object, properties );

}

}
}
}

#undef X