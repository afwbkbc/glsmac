#include "DefineAnimation.h"

#include "game/backend/Game.h"

#include "engine/Engine.h"
#include "loader/sound/SoundLoader.h"
#include "game/backend/animation/AnimationManager.h"
#include "game/backend/animation/Def.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

DefineAnimation::DefineAnimation( const size_t initiator_slot, animation::Def* def )
	: LegacyEvent( initiator_slot, ET_ANIMATION_DEFINE )
	, m_def( def ) {
	//
}

const std::string* DefineAnimation::Validate( GSE_CALLABLE, Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to define animations" );
	}
	if ( !g_engine->GetSoundLoader()->LoadCustomSound( m_def->m_sound_file ) ) {
		return Error( "Failed to load animation sound '" + m_def->m_sound_file + "'" );
	}
	return Ok();
}

gse::Value* const DefineAnimation::Apply( GSE_CALLABLE, Game* game ) const {
	game->GetAM()->DefineAnimation( m_def );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

TS_BEGIN( DefineAnimation )
		TS_FUNC_BEGIN( "DefineAnimation" ) +
			TS_FUNC_ARG( "def", TS_OF( m_def ) ) +
		TS_FUNC_END()
TS_END()

void DefineAnimation::Serialize( types::Buffer& buf, const DefineAnimation* event ) {
	buf.WriteString( animation::Def::Serialize( event->m_def ).ToString() );
}

DefineAnimation* DefineAnimation::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefineAnimation( initiator_slot, animation::Def::Unserialize( b ) );
}

}
}
}
