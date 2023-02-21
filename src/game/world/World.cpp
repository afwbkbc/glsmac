#include "World.h"

#include "engine/Engine.h"

#include "../mainmenu/MainMenu.h"

#include "graphics/Graphics.h"

#include "scene/actor/InstancedMesh.h"
#include "util/FS.h"

#ifdef MAPGEN_BENCHMARK
#include <iostream>
#include "map_generator/SimpleRandom.h"
#include "map_generator/SimpleRandomNoLoops.h"
#include "map_generator/SimpleRandomNoPointers.h"
#endif

#include "map_generator/SimplePerlin.h"
#include "map_generator/Test.h"

// TODO: move to settings
#define MAP_ROTATE_SPEED 2.0f

#define INITIAL_CAMERA_ANGLE { -M_PI * 0.5, M_PI * 0.75, 0 }

#ifdef DEBUG
#define MAP_FILENAME "./tmp/lastmap.gsm"
#define MAP_DUMP_FILENAME "./tmp/lastmap.dump"
#define MAP_SEED_FILENAME "./tmp/lastmap.seed"
#endif

namespace game {
namespace world {

const World::consts_t World::s_consts = {};

World::World( const Settings& settings )
	: m_settings( settings )
{
	NEW( m_random, Random );
#ifdef DEVEL
	//m_random->SetState( {3927071555,2047740357,9789330,4039274482} ); // had slope removal bug
	//m_random->SetState( {3199628599,474732328,355451379,1762265997} ); // weird light bug on 1 tile?
	//m_random->SetState( {946763678,2473140922,2270397686,4138960536} ); // another weird light bug
	//m_random->SetState( {70351513,643661934,2396482743,1984892262} ); // stretch bug
	//m_random->SetState( {3083264277,2759361294,2259827786,799043819} ); // lots of fungus
	//m_random->SetState( {2057306363,3857423773,2087610360,4043297429} ); // texture shift glitch
	//m_random->SetState( {2194870243,307068045,2446867731,2103210869} ); // long straight island
	//m_random->SetState( {3684284985,1052465576,1524137173,3726230539} ); // mirroring perlin coastlines
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

	m_world_scene->SetCamera( m_camera );
	
	// don't set exact 45 degree angles for lights, it will produce weird straight lines because of shadows
	{
		NEW( m_light_a, Light, Light::LT_AMBIENT_DIFFUSE );
		m_light_a->SetPosition( { 48.227f, 20.412f, 57.65f } );
		m_light_a->SetColor( { 0.8f, 0.9f, 1.0f, 0.8f } );
		m_world_scene->AddLight( m_light_a );
	}
	{
		NEW( m_light_b, Light, Light::LT_AMBIENT_DIFFUSE );
		m_light_b->SetPosition( { 22.412f, 62.227f, 43.35f } );
		m_light_b->SetColor( { 1.0f, 0.9f, 0.8f, 0.8f } );
		m_world_scene->AddLight( m_light_b );
	}
	
	g_engine->GetGraphics()->AddScene( m_world_scene );	
	
	NEW( m_map, Map, m_random, m_world_scene );
	
#ifdef MAPGEN_BENCHMARK
	NEWV( tiles, Tiles, 2400, 1600, m_random );
#else
	#ifdef DEVEL
		NEWV( tiles, Tiles, 40, 20, m_random );
	#else
		NEWV( tiles, Tiles, 80, 40, m_random );
	#endif
#endif
	//NEWV( tiles, Tiles, 200, 120, m_random );
	
	Log( "Map seed is " + m_random->GetStateString() );
	
#ifdef DEBUG
	// if crash happens - it's handy to have a seed to reproduce it
	FS::WriteFile( MAP_SEED_FILENAME, m_random->GetStateString() );
#endif
	
#ifdef DEVEL
	if ( FS::FileExists( MAP_DUMP_FILENAME ) ) {
		Log( (std::string) "Loading map dump from " + MAP_DUMP_FILENAME );
		m_map->SetTiles( tiles, false );
		m_map->Unserialize( Buffer( FS::ReadFile( MAP_DUMP_FILENAME ) ) );
	}
	else
#endif
	{
#ifdef DEVEL
		if ( FS::FileExists( MAP_FILENAME ) ) {
			Log( (std::string) "Loading map from " + MAP_FILENAME );
			tiles->Unserialize( Buffer( FS::ReadFile( MAP_FILENAME ) ) );
		}
		else
#endif
		{
			map_generator::SimplePerlin generator;
			//map_generator::SimpleRandom generator;
			//map_generator::SimpleRandomNoLoops generator;
			//map_generator::SimpleRandomNoPointers generator;
			//map_generator::Test generator;
#if defined(DEBUG) || defined(MAPGEN_BENCHMARK)
			util::Timer timer;
			timer.Start();
#endif
			generator.Generate( tiles, m_random->GetUInt( 0, UINT32_MAX - 1 ) );
#ifdef DEBUG
			Log( "Map generation took " + std::to_string( timer.GetElapsed().count() ) + "ms" );
#elif defined(MAPGEN_BENCHMARK)
			std::cout << "Map generation took " << std::to_string( timer.GetElapsed().count() ) << "ms" << std::endl;
#endif
#ifdef MAPGEN_BENCHMARK
			exit(0);
#endif
#ifdef DEBUG
			// if crash happens - it's handy to have a map file to reproduce it
			Log( (std::string) "Saving map to " + MAP_FILENAME );
			FS::WriteFile( MAP_FILENAME, tiles->Serialize().ToString() );
#endif
		}
		m_map->SetTiles( tiles );
#ifdef DEBUG
		// also handy to have dump of generated map
		Log( (std::string) "Saving map dump to " + MAP_DUMP_FILENAME );
		FS::WriteFile( MAP_DUMP_FILENAME, m_map->Serialize().ToString() );
#endif
	}
	
	auto* ui = g_engine->GetUI();
	
	// UI
	ui->AddTheme( &m_ui.theme );
	NEW( m_ui.bottom_bar, ui::BottomBar, this );
	ui->AddObject( m_ui.bottom_bar );

	m_viewport.bottom_bar_overlap = 32; // it has transparent area on top so let map render through it
	
	// map event handlers
	m_handlers.keydown = ui->AddGlobalEventHandler( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( !data->key.modifiers ) {
			
			if ( m_selected_tile_info.tile && m_selected_tile_info.ts ) {
				
				// move tile selector
				bool is_tile_moved = true;
				if ( data->key.code == UIEvent::K_LEFT || data->key.code == UIEvent::K_KP_LEFT ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->W;
					m_selected_tile_info.ts = m_selected_tile_info.ts->W;
				}
				else if ( data->key.code == UIEvent::K_UP || data->key.code == UIEvent::K_KP_UP ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->N;
					m_selected_tile_info.ts = m_selected_tile_info.ts->N;
				}
				else if ( data->key.code == UIEvent::K_RIGHT || data->key.code == UIEvent::K_KP_RIGHT ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->E;
					m_selected_tile_info.ts = m_selected_tile_info.ts->E;
				}
				else if ( data->key.code == UIEvent::K_DOWN || data->key.code == UIEvent::K_KP_DOWN ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->S;
					m_selected_tile_info.ts = m_selected_tile_info.ts->S;
				}
				else if ( data->key.code == UIEvent::K_HOME || data->key.code == UIEvent::K_KP_LEFT_UP ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->NW;
					m_selected_tile_info.ts = m_selected_tile_info.ts->NW;
				}
				else if (
					data->key.code == UIEvent::K_END || data->key.code == UIEvent::K_KP_LEFT_DOWN ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->SW;
					m_selected_tile_info.ts = m_selected_tile_info.ts->SW;
				}
				else if (
					data->key.code == UIEvent::K_PAGEUP || data->key.code == UIEvent::K_KP_RIGHT_UP ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->NE;
					m_selected_tile_info.ts = m_selected_tile_info.ts->NE;
				}
				else if (
					data->key.code == UIEvent::K_PAGEDOWN || data->key.code == UIEvent::K_KP_RIGHT_DOWN ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->SE;
					m_selected_tile_info.ts = m_selected_tile_info.ts->SE;
				}
				else {
					is_tile_moved = false; // not moved
				}
				
				if ( is_tile_moved ) {
					// moved
					SelectTile( m_selected_tile_info );
					auto tc = GetTileWindowCoordinates( m_selected_tile_info.ts );
					if (
						( fabs( -m_camera_position.x - tc.x ) > World::s_consts.map_scroll.key_scrolling.scroll_if_selected_tile_farther_from_center_than ) ||
						( fabs( -m_camera_position.y - tc.y ) > World::s_consts.map_scroll.key_scrolling.scroll_if_selected_tile_farther_from_center_than )
					) {
						ScrollToTile( m_selected_tile_info.ts );
					}
					return true;
				}
			}
			
			// exit to main menu
			if ( data->key.code == UIEvent::K_ESCAPE ) {
				ReturnToMainMenu();
				return true;
			}
		}
		return false;
	}, UI::GH_AFTER );
	
	m_handlers.mousedown = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_LEFT: {
				SelectTileAtPoint( data->mouse.absolute.x, data->mouse.absolute.y ); // async
				break;
			}
			case UIEvent::M_RIGHT: {
				m_smooth_scrolling.timer.Stop();
				m_map_control.is_dragging = true;
				m_map_control.last_drag_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
				break;
			}
#ifdef DEBUG
			case UIEvent::M_MIDDLE: {
				m_map_control.is_rotating = true;
				m_map_control.last_rotate_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
				break;
			}
#endif
		}
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mousemove = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this ) {
		if ( m_map_control.is_dragging ) {
				
			Vec2<float> current_drag_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
			Vec2<float> drag = current_drag_position - m_map_control.last_drag_position;
			
			m_camera_position.x += (float) drag.x;
			m_camera_position.y += (float) drag.y;
			UpdateCameraPosition();
			
			m_map_control.last_drag_position = current_drag_position;
		}
#ifdef DEBUG
		else if ( m_map_control.is_rotating ) {
			Vec2<float> current_rotate_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
			Vec2<float> rotate = current_rotate_position - m_map_control.last_rotate_position;
			
			for (auto& actor : m_map->GetActors() ) {
				auto newz = actor->GetAngleZ() + ( (float) rotate.x * MAP_ROTATE_SPEED );
				auto newy = std::max( -0.5f, std::min( 0.5f, actor->GetAngleY() + ( (float) rotate.y * MAP_ROTATE_SPEED ) ) );
				actor->SetAngleZ( newz );
				actor->SetAngleY( newy );
			}
			m_map_control.last_rotate_position = current_rotate_position;
		}
#endif
		else {
			const ssize_t edge_distance = m_viewport.is_fullscreen
				? World::s_consts.map_scroll.static_scrolling.edge_distance_px.fullscreen
				: World::s_consts.map_scroll.static_scrolling.edge_distance_px.windowed
			;
			if ( data->mouse.absolute.x < edge_distance ) {
				m_map_control.edge_scrolling.speed.x = World::s_consts.map_scroll.static_scrolling.speed.x;
			}
			else if ( data->mouse.absolute.x >= m_viewport.window_width - edge_distance ) {
				m_map_control.edge_scrolling.speed.x = -World::s_consts.map_scroll.static_scrolling.speed.x;
			}
			else {
				m_map_control.edge_scrolling.speed.x = 0;
			}
			if ( data->mouse.absolute.y <= edge_distance ) {
				m_map_control.edge_scrolling.speed.y = World::s_consts.map_scroll.static_scrolling.speed.y;
			}
			else if ( data->mouse.absolute.y >= m_viewport.window_height - edge_distance ) {
				m_map_control.edge_scrolling.speed.y = -World::s_consts.map_scroll.static_scrolling.speed.y;
			}
			else {
				m_map_control.edge_scrolling.speed.y = 0;
			}
			if ( m_map_control.edge_scrolling.speed ) {
				if ( !m_map_control.edge_scrolling.timer.Running() ) {
					Log( "Edge scrolling started" );
					m_map_control.edge_scrolling.timer.SetInterval( World::s_consts.map_scroll.static_scrolling.scroll_step_ms );
				}
			}
			else {
				if ( m_map_control.edge_scrolling.timer.Running() ) {
					Log( "Edge scrolling stopped" );
					m_map_control.edge_scrolling.timer.Stop();
				}
			}
		}
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mouseup = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_RIGHT: {
				m_map_control.is_dragging = false;
				break;
			}
#ifdef DEBUG
			case UIEvent::M_MIDDLE: {
				m_map_control.is_rotating = false;
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
		
		float speed = World::s_consts.map_scroll.smooth_scrolling.zoom_speed * m_camera_position.z;
		
		float new_z = m_camera_position.z + data->mouse.scroll_y * speed;
		
		if ( new_z < m_camera_range.min.z ) {
			new_z = m_camera_range.min.z;
		}
		if ( new_z > m_camera_range.max.z ) {
			new_z = m_camera_range.max.z;
		}
		
		float diff = m_camera_position.z / new_z;
		
		ScrollTo({
			m_camera_position.x / diff,
			m_camera_position.y / diff,
			new_z
		});
		
/*		m_camera_position.z = new_z;
		UpdateCameraScale();
		
		m_camera_position.x /= diff;
		m_camera_position.y /= diff;

		UpdateCameraRange();
		*/
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
	
	UpdateUICamera();
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
	DELETE( m_light_a );
	DELETE( m_light_b );
	DELETE( m_world_scene );
}

void World::Iterate() {
	
	// response for clicked tile (if click happened)
	auto tile_info = m_map->GetTileAtScreenCoordsResult();
	if ( tile_info.tile ) {
		SelectTile( tile_info );
		CenterAtTile( tile_info.ts );
	}
	
	bool is_camera_position_updated = false;
	bool is_camera_scale_updated = false;
	while ( m_map_control.edge_scrolling.timer.Ticked() ) {
		m_camera_position.x += m_map_control.edge_scrolling.speed.x;
		m_camera_position.y += m_map_control.edge_scrolling.speed.y;
		is_camera_position_updated = true;
	}
	
	while ( m_smooth_scrolling.timer.Ticked() ) {
		ASSERT( m_smooth_scrolling.steps_left, "no scrolling steps but timer running" );
		m_camera_position += m_smooth_scrolling.step;
		if ( !--m_smooth_scrolling.steps_left ) {
			Log( "Scrolling finished" );
			m_smooth_scrolling.timer.Stop();
			break;
		}
		is_camera_position_updated = true;
		if ( m_smooth_scrolling.step.z != 0 ) {
			is_camera_scale_updated = true;
		}
	}
	
	if ( is_camera_scale_updated ) {
		UpdateCameraScale();
		UpdateCameraRange();
	}
	if ( is_camera_position_updated ) {
		UpdateCameraPosition();
	}
	
	for ( auto& actor : m_actors_vec ) {
		actor->Iterate();
	}
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
	auto* graphics = g_engine->GetGraphics();
	m_viewport.window_width = graphics->GetViewportWidth();
	m_viewport.window_height = graphics->GetViewportHeight();
	m_viewport.max.x = m_viewport.window_width;
	m_viewport.max.y = m_viewport.window_height - m_ui.bottom_bar->GetHeight() + m_viewport.bottom_bar_overlap;
	m_viewport.ratio.x = (float) m_viewport.window_width / m_viewport.max.x;
	m_viewport.ratio.y = (float) m_viewport.window_height / m_viewport.max.y;
	m_viewport.width = m_viewport.max.x - m_viewport.min.x;
	m_viewport.height = m_viewport.max.y - m_viewport.min.y;
	m_viewport.aspect_ratio = (float) m_viewport.width / m_viewport.height;
	m_viewport.viewport_aspect_ratio = graphics->GetAspectRatio();
	m_viewport.is_fullscreen = graphics->IsFullscreen();
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
	//UpdateUICamera();
}

void World::UpdateCameraAngle() {
	m_camera->SetAngle( m_camera_angle );
	//UpdateUICamera();
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
	std::vector< Vec3 > instances;
	
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

void World::UpdateUICamera() {
	// TODO: finish it
	// snapshot camera matrix for world ui
	/*m_camera->GetMatrix()*/
	// tmp/hack
	/*for ( auto& a : m_map->GetActors() ) {
		for ( auto& m : ((scene::actor::InstancedMesh*)a)->GetWorldMatrices() ) {
			g_engine->GetUI()->SetWorldUIMatrix( m );
			break;
		}
		break;
	}*/
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

void World::SelectTileAtPoint( const size_t x, const size_t y ) {
	Log( "Looking up tile at " + std::to_string( x ) + "x" + std::to_string( y ) );
	m_map->GetTileAtScreenCoords( x, m_viewport.window_height - y ); // async
}

void World::SelectTile( const Map::tile_info_t& tile_info ) {
	DeselectTile();
	auto tile = tile_info.tile;
	auto ts = tile_info.ts;
	
	Log( "Selecting tile at " + std::to_string( tile->coord.x ) + "x" + std::to_string( tile->coord.y ) );
	Map::tile_layer_type_t lt = ( tile->is_water_tile ? Map::LAYER_WATER : Map::LAYER_LAND );
	auto& layer = ts->layers[ lt ];
	auto coords = layer.coords;
	
	if ( !tile->is_water_tile && ts->is_coastline_corner ) {
		if ( tile->W->is_water_tile ) {
			coords.left = ts->layers[ Map::LAYER_WATER ].coords.left;
		}
		if ( tile->N->is_water_tile ) {
			coords.top = ts->layers[ Map::LAYER_WATER ].coords.top;
		}
		if ( tile->E->is_water_tile ) {
			coords.right = ts->layers[ Map::LAYER_WATER ].coords.right;
		}
		if ( tile->S->is_water_tile ) {
			coords.bottom = ts->layers[ Map::LAYER_WATER ].coords.bottom;
		}
	}
	
	NEW( m_actors.tile_selection, actor::TileSelection, coords );
	AddActor( m_actors.tile_selection );
	
	m_ui.bottom_bar->PreviewTile( m_map, tile_info );
	
	m_selected_tile_info = tile_info;
}

void World::DeselectTile() {
	if ( m_actors.tile_selection ) {
		RemoveActor( m_actors.tile_selection );
		m_actors.tile_selection = nullptr;
	}
	
	m_ui.bottom_bar->HideTilePreview();
}

void World::AddActor( actor::Actor* actor ) {
	ASSERT( m_actors_vec.find( actor ) == m_actors_vec.end(), "world actor already added" );
	m_actors_vec.insert( actor );
	m_world_scene->AddActor( actor );
}

void World::RemoveActor( actor::Actor* actor ) {
	auto it = m_actors_vec.find( actor );
	ASSERT( it != m_actors_vec.end(), "world actor not found" );
	m_actors_vec.erase( it );
	m_world_scene->RemoveActor( actor );
	DELETE( actor );
}

const Vec2< float > World::GetTileWindowCoordinates( const Map::tile_state_t* ts ) {
	return {
		ts->coord.x / m_viewport.viewport_aspect_ratio * m_camera_position.z,
		( ts->coord.y - std::max( 0.0f, Map::s_consts.clampers.elevation_to_vertex_z.Clamp( ts->elevations.center ) ) ) * m_viewport.ratio.y * m_camera_position.z / 1.414f
	};
}

void World::ScrollTo( const Vec3& target ) {
	m_smooth_scrolling.target_position = target;
	m_smooth_scrolling.steps_left = World::s_consts.map_scroll.smooth_scrolling.scroll_steps;
	m_smooth_scrolling.step = ( m_smooth_scrolling.target_position - m_camera_position ) / m_smooth_scrolling.steps_left;
	m_smooth_scrolling.timer.SetInterval( World::s_consts.map_scroll.smooth_scrolling.scroll_step_ms );
	Log( "Scrolling from " + m_camera_position.ToString() + " to " + m_smooth_scrolling.target_position.ToString() );
}

void World::ScrollToTile( const Map::tile_state_t* ts ) {
	auto tc = GetTileWindowCoordinates( ts );
	
	const float tile_x_shifted = m_camera_position.x > 0
		? tc.x - ( m_camera_range.max.x - m_camera_range.min.x )
		: tc.x + ( m_camera_range.max.x - m_camera_range.min.x )
	;
	if (
		fabs( tile_x_shifted - -m_camera_position.x )
			<
		fabs( tc.x - -m_camera_position.x )
	) {
		// smaller distance if going other side
		tc.x = tile_x_shifted;
	}
	
	ScrollTo({
		- tc.x,
		- tc.y,
		m_camera_position.z
	});
}

void World::CenterAtTile( const Map::tile_state_t* ts ) {
	ScrollToTile( ts );
}

}
}
