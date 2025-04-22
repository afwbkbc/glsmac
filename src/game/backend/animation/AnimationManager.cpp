#include "AnimationManager.h"

#include "game/backend/Game.h"
#include "Def.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Float.h"
#include "gse/ExecutionPointer.h"
#include "engine/Engine.h"
#include "loader/sound/SoundLoader.h"
#include "game/backend/animation/FramesRow.h"

namespace game {
namespace backend {
namespace animation {

AnimationManager::AnimationManager( Game* game )
	: gse::GCWrappable( game->GetGCSpace() )
	, m_game( game ) {
	//
}

AnimationManager::~AnimationManager() {
	Clear();
}

void AnimationManager::Clear() {
	m_running_animations_callbacks.clear();
	m_next_running_animation_id = 0;
	for ( auto& it : m_animation_defs ) {
		delete it.second;
	}
	m_animation_defs.clear();
}

void AnimationManager::DefineAnimation( animation::Def* def ) {
	Log( "Defining animation ('" + def->m_id + "')" );

	ASSERT( m_animation_defs.find( def->m_id ) == m_animation_defs.end(), "animation definition already exists" );

	// backend doesn't need any animation details, just keep track of it's existence for validations
	m_animation_defs.insert(
		{
			def->m_id,
			def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_ANIMATION_DEFINE );
	NEW( fr.data.animation_define.serialized_animation, std::string, animation::Def::Serialize( def ).ToString() );
	m_game->AddFrontendRequest( fr );
}

void AnimationManager::UndefineAnimation( const std::string& id ) {
	Log( "Undefining animation ('" + id + "')" );

	ASSERT( m_animation_defs.find( id ) != m_animation_defs.end(), "animation definition not found" );

	m_animation_defs.erase( id );

	auto fr = FrontendRequest( FrontendRequest::FR_ANIMATION_UNDEFINE );
	NEW( fr.data.animation_undefine.animation_id, std::string, id );
	m_game->AddFrontendRequest( fr );
}

const std::string* AnimationManager::ShowAnimation( const std::string& animation_id, map::tile::Tile* tile, const cb_oncomplete& on_complete ) {
	if ( m_animation_defs.find( animation_id ) == m_animation_defs.end() ) {
		return new std::string( "Animation '" + animation_id + "' is not defined" );
	}
	if ( !tile->IsLocked() ) {
		return new std::string( "Tile must be locked before showing animation" );
	}
	auto fr = FrontendRequest( FrontendRequest::FR_ANIMATION_SHOW );
	NEW( fr.data.animation_show.animation_id, std::string, animation_id );
	fr.data.animation_show.running_animation_id = AddAnimationCallback( on_complete );
	const auto c = m_game->GetTileRenderCoords( tile );
	fr.data.animation_show.render_coords = {
		c.x,
		c.y,
		c.z,
	};
	m_game->AddFrontendRequest( fr );
	return nullptr; // no error
}

void AnimationManager::FinishAnimation( const size_t animation_id ) {
	const auto& it = m_running_animations_callbacks.find( animation_id );
	ASSERT( it != m_running_animations_callbacks.end(), "animation " + std::to_string( animation_id ) + " is not running" );
	const auto on_complete = it->second;
	m_running_animations_callbacks.erase( it );
	on_complete();
}

WRAPIMPL_BEGIN( AnimationManager )
	WRAPIMPL_PROPS
	WRAPIMPL_TRIGGERS
		{
			"define",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( animation_def, 1, Object );
				N_GETPROP( type, animation_def, "type", String );

				if ( m_animation_defs.find( id ) != m_animation_defs.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Animation \"" + id + "\" already exists" );
				}

				if ( type == "frames_row" ) {
					N_GETPROP( file, animation_def, "file", String );
					N_GETPROP( row_x, animation_def, "row_x", Int );
					N_GETPROP( row_y, animation_def, "row_y", Int );
					N_GETPROP( frame_width, animation_def, "frame_width", Int );
					N_GETPROP( frame_height, animation_def, "frame_height", Int );
					N_GETPROP_OPT( int64_t, frame_center_x, animation_def, "frame_center_x", Int, frame_width / 2 );
					N_GETPROP_OPT( int64_t, frame_center_y, animation_def, "frame_center_y", Int, frame_height / 2 );
					N_GETPROP( frame_padding, animation_def, "frame_padding", Int );
					N_GETPROP( frames_count, animation_def, "frames_count", Int );
					N_GETPROP_OPT( int64_t, frames_per_row, animation_def, "frames_per_row", Int, frames_count );
					N_GETPROP_OPT( float, scale_x, animation_def, "scale_x", Float, 1.0f );
					N_GETPROP_OPT( float, scale_y, animation_def, "scale_y", Float, 1.0f );
					N_GETPROP( duration_ms, animation_def, "duration_ms", Int );
					N_GETPROP( sound, animation_def, "sound", String );
					if ( !g_engine->GetSoundLoader()->LoadCustomSound( sound ) ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Failed to load animation sound '" + sound + "'" );
					}
					auto* def = new animation::FramesRow(
						id,
						file,
						row_x,
						row_y,
						frame_width,
						frame_height,
						frame_center_x,
						frame_center_y,
						frame_padding,
						frames_count,
						frames_per_row,
						scale_x,
						scale_y,
						duration_ms,
						sound
					);
					DefineAnimation( def );
					return VALUE( gse::value::Undefined );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported animation type: " + type );
				}
			})
			},
		{
			"show_animation",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 3 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );
				N_GET_CALLABLE( on_complete, 2 );
				Persist( on_complete );
				const auto* errmsg = ShowAnimation( id, tile, [ this, on_complete, gc_space, ctx, si, ep ]() {
					auto ep2 = ep;
					on_complete->Run( gc_space, ctx, si, ep2, {} );
					Unpersist( on_complete );
				});
				if ( errmsg ) {
					GSE_ERROR( gse::EC.GAME_ERROR, *errmsg );
					Unpersist( on_complete );
					delete errmsg;
				}

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"undefine",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );

				if ( m_animation_defs.find( id ) == m_animation_defs.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Animation \"" + id + "\" does not exist" );
				}

				UndefineAnimation( id );

				return VALUE( gse::value::Undefined );
			} )
		}
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( AnimationManager )

void AnimationManager::Serialize( types::Buffer& buf ) const {
	Log( "Serializing " + std::to_string( m_animation_defs.size() ) + " animation defs" );
	buf.WriteInt( m_animation_defs.size() );
	for ( const auto& it : m_animation_defs ) {
		buf.WriteString( it.first );
		buf.WriteString( animation::Def::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( m_next_running_animation_id );
	Log( "Saved next animation id: " + std::to_string( m_next_running_animation_id ) );
}

void AnimationManager::Unserialize( types::Buffer& buf ) {
	ASSERT( m_animation_defs.empty(), "animation defs not empty" );
	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " animation defs" );
	m_animation_defs.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineAnimation( animation::Def::Unserialize( b ) );
	}
	m_next_running_animation_id = buf.ReadInt();
	Log( "Restored next animation id: " + std::to_string( m_next_running_animation_id ) );
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string AnimationManager::ToString() {
	return "game::AnimationManager()";
}
#endif

const size_t AnimationManager::AddAnimationCallback( const cb_oncomplete& on_complete ) {
	const auto running_animation_id = ++m_next_running_animation_id;
	m_running_animations_callbacks.insert(
		{
			running_animation_id,
			on_complete
		}
	);
	return running_animation_id;
}

}
}
}
