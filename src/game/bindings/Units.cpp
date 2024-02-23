#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"

#include "game/unit/StaticDef.h"
#include "game/unit/SpriteRender.h"

#include "game/event/DefineUnit.h"
#include "game/event/SpawnUnit.h"
#include "game/event/DespawnUnit.h"

namespace game {
namespace bindings {

const unit::Unit::morale_t GetMorale( const int64_t& morale, gse::Context* ctx, const gse::si_t& call_si ) {
	if ( morale < unit::Unit::MORALE_MIN || morale > unit::Unit::MORALE_MAX ) {
		ERROR( gse::EC.INVALID_CALL, "Invalid morale value: " + std::to_string( morale ) + " (should be between " + std::to_string( unit::Unit::MORALE_MIN ) + " and " + std::to_string( unit::Unit::MORALE_MAX ) + ", inclusive)" );
	}
	return (unit::Unit::morale_t)morale;
}

const unit::Unit::health_t GetHealth( const float health, gse::Context* ctx, const gse::si_t& call_si ) {
	if ( health < unit::Unit::HEALTH_MIN || health > unit::Unit::HEALTH_MAX ) {
		ERROR( gse::EC.INVALID_CALL, "Invalid health value: " + std::to_string( health ) + " (should be between " + std::to_string( unit::Unit::HEALTH_MIN ) + " and " + std::to_string( unit::Unit::HEALTH_MAX ) + ", inclusive)" );
	}
	if ( health == 0 ) {
		ERROR( gse::EC.INVALID_CALL, "Invalid health value: " + std::to_string( health ) + " (you can't spawn a dead unit)" );
	}
	return (unit::Unit::health_t)health;
}

BINDING_IMPL( units ) {
	const gse::type::Object::properties_t properties = {
		{
			"define",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( unit_def, 1, Object );
				N_GETPROP( name, unit_def, "name", String );
				N_GETPROP( unit_type, unit_def, "type", String );
				if ( unit_type == "static" ) {
					N_GETPROP( render_def, unit_def, "render", Object );
					N_GETPROP( render_type, render_def, "type", String );
					if ( render_type == "sprite" ) {
						N_GETPROP( sprite_file, render_def, "file", String );
						N_GETPROP( sprite_x, render_def, "x", Int );
						N_GETPROP( sprite_y, render_def, "y", Int );
						N_GETPROP( sprite_w, render_def, "w", Int );
						N_GETPROP( sprite_h, render_def, "h", Int );
						N_GETPROP( sprite_cx, render_def, "cx", Int );
						N_GETPROP( sprite_cy, render_def, "cy", Int );
						N_GETPROP_OPT_INT( sprite_morale_based_xshift, render_def, "morale_based_xshift" );
						const auto* def = new unit::StaticDef(
							id,
							name,
							new unit::SpriteRender( sprite_file, sprite_x, sprite_y, sprite_w, sprite_h, sprite_cx, sprite_cy, sprite_morale_based_xshift )
						);
						auto* game = GAME;
						return game->AddGameEvent( new event::DefineUnit( game->GetInitiatorSlot(), def ), ctx, call_si );
					}
					else {
						ERROR( gse::EC.GAME_ERROR, "Unsupported render type: " + render_type );
					}
				}
				else {
					ERROR( gse::EC.GAME_ERROR, "Unsupported unit type: " + unit_type );
				}
				return VALUE( gse::type::Undefined );
			})
		},
		{
			"spawn",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 5 );
				N_GETVALUE( def_name, 0, String );
				N_UNWRAP( owner, 1, Slot );
				N_UNWRAP( tile, 2, map::Tile );
				N_GETVALUE( morale, 3, Int );
				N_GETVALUE( health, 4, Float );
				auto* game = GAME;
				return game->AddGameEvent( new event::SpawnUnit(
					game->GetInitiatorSlot(),
					def_name,
					owner->GetIndex(),
					tile->coord.x,
					tile->coord.y,
					GetMorale( morale, ctx, call_si ),
					GetHealth( health, ctx, call_si )
				), ctx, call_si );
			})
		},
		{
			"despawn",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_UNWRAP( unit, 0, unit::Unit );
				auto* game = GAME;
				return game->AddGameEvent( new event::DespawnUnit( game->GetInitiatorSlot(), unit->m_id ), ctx, call_si );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
