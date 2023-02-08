#include "World.h"
#include "engine/Engine.h"

#include "../mainmenu/MainMenu.h"

#include "graphics/Graphics.h"

#include "types/mesh/Rectangle.h"
#include "util/FS.h"

#include "map_generator/SimpleRandom.h"
#include "map_generator/SimplePerlin.h"
#include "map_generator/Test.h"

// TODO: move to settings
#define MAP_SCROLL_SPEED 1.0f
#define MAP_ROTATE_SPEED 2.0f
#define MAP_ZOOM_SPEED 0.1f

#define INITIAL_CAMERA_ANGLE { -M_PI * 0.5, M_PI * 0.75, 0 }

#ifdef DEBUG
#define MAP_FILENAME "./tmp/lastmap.gsm"
#define MAP_DUMP_FILENAME "./tmp/lastmap.dump"
#define MAP_SEED_FILENAME "./tmp/lastmap.seed"
#endif

namespace game {
namespace world {

World::World( const Settings& settings )
	: m_settings( settings )
{
	NEW( m_random, Random );
#ifdef DEVEL
	//m_random->SetState(  );
#endif
}

World::~World() {
	DELETE( m_random );
}

void World::Start() {
	
	NEW( m_world_scene, Scene, "World", SCENE_TYPE_ORTHO );
	
	NEW( m_camera, Camera, Camera::CT_ORTHOGRAPHIC );
	m_camera_angle = INITIAL_CAMERA_ANGLE;
	UpdateCameraAngle();

	NEW( m_light, Light, Light::LT_AMBIENT_DIFFUSE );
	m_light->SetPosition( { 50.0f, 50.0f, 30.0f } );
	m_light->SetColor( { 1.0f, 1.0f, 1.0f, 0.5f } );
	
	m_world_scene->SetCamera( m_camera );
	m_world_scene->SetLight( m_light );
	g_engine->GetGraphics()->AddScene( m_world_scene );	
	
	NEW( m_map, Map, m_random, m_world_scene );
	
#ifdef DEVEL
	NEWV( tiles, Tiles, 40, 20, m_random );
#else
	NEWV( tiles, Tiles, 80, 40, m_random );
#endif
	//NEWV( tiles, Tiles, 200, 120, m_random );
	
	Log( "Map seed is " + m_random->GetStateString() );
	
#ifdef DEBUG
	// if crash happens - it's handy to have a seed to reproduce it
	FS::WriteFile( MAP_SEED_FILENAME, m_random->GetStateString() );
#endif
	
#ifdef DEVEL
	if ( FS::FileExists( MAP_DUMP_FILENAME ) ) {
		Log( (string) "Loading map dump from " + MAP_DUMP_FILENAME );
		m_map->SetTiles( tiles, false );
		m_map->Unserialize( Buffer( FS::ReadFile( MAP_DUMP_FILENAME ) ) );
	}
	else
#endif
	{
#ifdef DEVEL
		if ( FS::FileExists( MAP_FILENAME ) ) {
			Log( (string) "Loading map from " + MAP_FILENAME );
			tiles->Unserialize( Buffer( FS::ReadFile( MAP_FILENAME ) ) );
		}
		else
#endif
		{
			map_generator::SimplePerlin generator;
			//map_generator::SimpleRandom generator;
			//map_generator::Test generator;
			generator.Generate( tiles, m_random->GetUInt( 0, UINT32_MAX - 1 ) );
#ifdef DEBUG
			// if crash happens - it's handy to have a map file to reproduce it
			Log( (string) "Saving map to " + MAP_FILENAME );
			FS::WriteFile( MAP_FILENAME, tiles->Serialize().ToString() );
#endif
		}
		m_map->SetTiles( tiles );
#ifdef DEBUG
		// also handy to have dump of generated map
		Log( (string) "Saving map dump to " + MAP_DUMP_FILENAME );
		FS::WriteFile( MAP_DUMP_FILENAME, m_map->Serialize().ToString() );
#endif
	}
	
	auto* ui = g_engine->GetUI();
	
	// UI
	ui->AddTheme( &m_ui.theme );
	NEW( m_ui.bottom_bar, ui::BottomBar, this );
	ui->AddObject( m_ui.bottom_bar );

	// map event handlers
	m_handlers.keydown = ui->AddGlobalEventHandler( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( !data->key.modifiers && data->key.code == UIEvent::K_ESCAPE ) {
			ReturnToMainMenu();
			return true;
		}
		return false;
	}, UI::GH_AFTER );
	
	m_handlers.mousedown = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_RIGHT: {
				m_is_dragging = true;
				m_last_drag_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
				break;
			}
#ifdef DEBUG
			case UIEvent::M_MIDDLE: {
				m_is_rotating = true;
				m_last_rotate_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
				break;
			}
#endif
		}
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mousemove = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this ) {
		if ( m_is_dragging ) {
			Vec2<float> current_drag_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
			Vec2<float> drag = current_drag_position - m_last_drag_position;
			
			m_camera_position.x += (float) drag.x * MAP_SCROLL_SPEED;
			m_camera_position.y += (float) drag.y * MAP_SCROLL_SPEED;
			UpdateCameraPosition();
			
			m_last_drag_position = current_drag_position;
		}
#ifdef DEBUG
		if ( m_is_rotating ) {
			Vec2<float> current_rotate_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
			Vec2<float> rotate = current_rotate_position - m_last_rotate_position;
			
			for (auto& actor : m_map->GetActors() ) {
				auto newz = actor->GetAngleZ() + ( (float) rotate.x * MAP_ROTATE_SPEED );
				auto newy = max( -0.5f, min( 0.5f, actor->GetAngleY() + ( (float) rotate.y * MAP_ROTATE_SPEED ) ) );
				actor->SetAngleZ( newz );
				actor->SetAngleY( newy );
			}
			m_last_rotate_position = current_rotate_position;
		}
#endif
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mouseup = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_RIGHT: {
				m_is_dragging = false;
				break;
			}
#ifdef DEBUG
			case UIEvent::M_MIDDLE: {
				m_is_rotating = false;
				for (auto& actor : m_map->GetActors() ) {
					actor->SetAngle( { 0.0, 0.0, 0.0 } );
				}
				break;
			}
#endif
		}
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mousescroll = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_SCROLL, EH( this ) {
		
		float speed = (float) MAP_ZOOM_SPEED * m_camera_position.z;
		
		float new_z = m_camera_position.z + (float) data->mouse.scroll_y * speed;
		
		if ( new_z < m_camera_range.min.z ) {
			new_z = m_camera_range.min.z;
		}
		if ( new_z > m_camera_range.max.z ) {
			new_z = m_camera_range.max.z;
		}
		
		float diff = m_camera_position.z / new_z;
		
		m_camera_position.z = new_z;
		UpdateCameraScale();
		
		m_camera_position.x /= diff;
		m_camera_position.y /= diff;

		UpdateCameraRange();
		
		return true;
	}, UI::GH_AFTER );
	
	// other stuff
	
	m_clamp.x.SetDstRange( -0.5f, 0.5f );
	m_clamp.y.SetDstRange( -0.5f, 0.5f );
	
	// map should continue scrolling even if mouse is outside viewport
	m_clamp.x.SetOverflowAllowed( true );
	m_clamp.y.SetOverflowAllowed( true );
	
	UpdateViewport();
	
	SetCameraPosition( { 0.0f, -0.25f, 0.1f } );

	UpdateCameraRange();
	UpdateCameraScale();
	
	g_engine->GetGraphics()->AddOnResizeHandler( this, RH( this ) {
		UpdateViewport();
		UpdateCameraRange();
		UpdateMapInstances();
	});
	
	UpdateMapInstances();
}

void World::Stop() {
	auto* ui = g_engine->GetUI();
	ui->RemoveObject( m_ui.bottom_bar );
	ui->RemoveTheme( &m_ui.theme );
	
	DELETE( m_map );
	
	g_engine->GetGraphics()->RemoveOnResizeHandler( this );
	
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.keydown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousedown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousemove );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mouseup );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousescroll );
	
	g_engine->GetGraphics()->RemoveScene( m_world_scene );	
	DELETE( m_camera );
	DELETE( m_light );
	DELETE( m_world_scene );
}

void World::Iterate() {
	
}

void World::SetCameraPosition( const Vec3 camera_position ) {
	if ( camera_position != m_camera_position ) {
		bool position_updated =
			( m_camera_position.x != camera_position.x ) ||
			( m_camera_position.y != camera_position.y )
		;
		bool scale_updated = m_camera_position.z != camera_position.z;
		m_camera_position = camera_position;
		if ( position_updated ) {
			UpdateCameraPosition();
		}
		if ( scale_updated ) {
			UpdateCameraScale();
		}
	}
}

void World::UpdateViewport() {
	m_viewport.max.x = g_engine->GetGraphics()->GetViewportWidth();
	m_viewport.max.y = g_engine->GetGraphics()->GetViewportHeight() - m_ui.bottom_bar->GetHeight() + 32; // bottom bar has some transparent area at top
	m_viewport.ratio.x = (float) g_engine->GetGraphics()->GetViewportWidth() / m_viewport.max.x;
	m_viewport.ratio.y = (float) g_engine->GetGraphics()->GetViewportHeight() / m_viewport.max.y;
	m_viewport.width = m_viewport.max.x - m_viewport.min.x;
	m_viewport.height = m_viewport.max.y - m_viewport.min.y;
	m_viewport.aspect_ratio = (float) m_viewport.width / m_viewport.height;
	m_viewport.viewport_aspect_ratio = g_engine->GetGraphics()->GetAspectRatio();
	m_clamp.x.SetSrcRange( m_viewport.min.x, m_viewport.max.x );
	m_clamp.y.SetSrcRange( m_viewport.min.y, m_viewport.max.y );
}

void World::UpdateCameraPosition() {
	
	// prevent vertical scrolling outside viewport
	if ( m_camera_position.y < m_camera_range.min.y ) {
		m_camera_position.y = m_camera_range.min.y;
	}
	if ( m_camera_position.y > m_camera_range.max.y ) {
		m_camera_position.y = m_camera_range.max.y;
		if ( m_camera_position.y < m_camera_range.min.y ) {
			m_camera_position.y = ( m_camera_range.min.y + m_camera_range.max.y ) / 2;
		}
	}
	
	// shift x between instances for infinite horizontal scrolling
	if ( m_camera_position.x < m_camera_range.min.x ) {
		m_camera_position.x = m_camera_range.max.x;
	}	
	else if ( m_camera_position.x > m_camera_range.max.x ) {
		m_camera_position.x = m_camera_range.min.x;
	}
	
	m_camera->SetPosition({
		( 0.5f + m_camera_position.x ) * m_viewport.viewport_aspect_ratio,
		( 0.5f + m_camera_position.y ) / m_viewport.ratio.y + Map::s_consts.tile_scale_z * m_camera_position.z / 1.41f, // TODO: why 1.41?
		( 0.5f + m_camera_position.y ) / m_viewport.ratio.y + m_camera_position.z
	});
}

void World::UpdateCameraScale() {
	m_camera->SetScale( { m_camera_position.z, m_camera_position.z, m_camera_position.z } );
}

void World::UpdateCameraAngle() {
	m_camera->SetAngle( m_camera_angle );
}

void World::UpdateCameraRange() {
	m_camera_range.min.z = 2.82f / ( m_map->GetHeight() + 1 ) / m_viewport.ratio.y; // TODO: why 2.82?
	m_camera_range.max.z = 0.22f; // TODO: fix camera z and allow to zoom in more
	if ( m_camera_position.z < m_camera_range.min.z ) {
		m_camera_position.z = m_camera_range.min.z;
	}
	if ( m_camera_position.z > m_camera_range.max.z ) {
		m_camera_position.z = m_camera_range.max.z;
	}
	m_camera_range.max.y = ( m_camera_position.z - m_camera_range.min.z ) * ( m_map->GetHeight() + 1 ) * m_viewport.ratio.y * 0.1768f; // TODO: why 0.1768?
	m_camera_range.min.y = -m_camera_range.max.y;
	
	//Log( "Camera range change: Z=[" + to_string( m_camera_range.min.z ) + "," + to_string( m_camera_range.max.z ) + "] Y=[" + to_string( m_camera_range.min.y ) + "," + to_string( m_camera_range.max.y ) + "], z=" + to_string( m_camera_position.z ) );
	
	m_camera_range.max.x = ( m_map->GetWidth() ) * m_camera_position.z / m_viewport.viewport_aspect_ratio * 0.25f;
	m_camera_range.min.x = -m_camera_range.max.x;
	
	UpdateCameraPosition();
	UpdateCameraScale();
}

void World::UpdateMapInstances() {
	// needed for horizontal scrolling
	vector< Vec3 > instances;
	
	const float mhw = Map::s_consts.tile.scale.x * m_map->GetWidth() / 2;
	
	uint8_t instances_before_after = floor( m_viewport.aspect_ratio / ( (float) m_map->GetWidth() / m_map->GetHeight() ) / 2 ) + 1;
	
	// don't spawn more instances than graphics system can support
	while ( instances_before_after * 2 + 1 > graphics::Graphics::MAX_WORLD_INSTANCES ) {
		instances_before_after--;
	}
	
	for ( uint8_t i = instances_before_after ; i > 0 ; i-- ) {
		instances.push_back( { -mhw * i, 0.0f, 0.0f } );
	}
	instances.push_back( { 0.0f, 0.0f, 0.0f} );
	for ( uint8_t i = 1 ; i <= instances_before_after ; i++ ) {
		instances.push_back( { +mhw * i, 0.0f, 0.0f } );
	}
	
	m_world_scene->SetInstances( instances );
}

void World::ReturnToMainMenu() {	
	
#ifdef DEVEL
	g_engine->ShutDown();
#else
	NEWV( task, game::mainmenu::MainMenu );
	g_engine->GetScheduler()->RemoveTask( this );
	g_engine->GetScheduler()->AddTask( task );
#endif
	
}

}
}
