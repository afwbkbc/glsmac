#include "Binding.h"

#include "Bindings.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Array.h"
#include "gse/type/Bool.h"
#include "gse/context/Context.h"
#include "game/backend/Game.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/base/PopDef.h"
#include "game/backend/event/DefinePop.h"
#include "game/backend/event/SpawnBase.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( bases ) {
	const gse::type::object_properties_t properties = {
		{
			"define_pop",
			NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( id, 0, String );
			N_GETVALUE( def, 1, Object );

			N_GETPROP( name, def, "name", String );

			base::pop_render_infos_t rh = {};
			base::pop_render_infos_t rp = {};
			const auto& f_read_renders = [ &def, &arg, &call_si, &ctx, &getprop_val, &obj_it ]( const std::string& key, base::pop_render_infos_t& out ) {
				N_GETPROP( renders, def, key, Array );
				out.reserve( renders.size() );
				for ( const auto& v : renders ) {
					if ( v.Get()->type != gse::type::Type::T_OBJECT ) {
						ERROR( gse::EC.INVALID_CALL, "Pop render elements must be objects" );
					}
					const auto* obj = (gse::type::Object*)v.Get();
					const auto& ov = obj->value;
					N_GETPROP( type, ov, "type", String );
					if ( type == "sprite" ) {
						N_GETPROP( file, ov, "file", String );
						N_GETPROP( x, ov, "x", Int );
						N_GETPROP( y, ov, "y", Int );
						N_GETPROP( w, ov, "w", Int );
						N_GETPROP( h, ov, "h", Int );
						out.push_back(
							base::pop_render_info_t{
								file,
								(uint16_t)x,
								(uint16_t)y,
								(uint16_t)w,
								(uint16_t)h
							}
						);
					}
					else {
						ERROR( gse::EC.INVALID_CALL, "Only sprite pops are supported for now" );
					}

				}
			};
			f_read_renders( "renders_human", rh );
			f_read_renders( "renders_progenitor", rp );

			base::PopDef::pop_flags_t flags = base::PopDef::PF_NONE;
			N_GETPROP_OPT( bool, can_work_tiles, def, "tile_worker", Bool, false );
			if ( can_work_tiles ) {
				flags |= base::PopDef::PF_TILE_WORKER;
			}

			auto* game = GAME;
			return game->AddEvent( new event::DefinePop(
				game->GetSlotNum(),
				new base::PopDef( id, name, rh, rp, flags )
			) );
		} )
		},
		{
			"spawn",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS_MIN_MAX( 3, 4 );
				N_GETVALUE_UNWRAP( owner, 0, slot::Slot );
				N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );

				N_GETVALUE( info, 2, Object );
				N_GETPROP_OPT( std::string, name, info, "name", String, "" );

				if ( arguments.size() > 3 ) {
					N_PERSIST_CALLABLE( on_spawn, 3 );
				}

				auto* game = GAME;
				return game->AddEvent( new event::SpawnBase(
					game->GetSlotNum(),
					owner->GetIndex(),
					tile->coord.x,
					tile->coord.y,
					name
				) );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
}
