#include "World.h"
#include "engine/Engine.h"

#include "../mainmenu/MainMenu.h"

#include "scene/mesh/Rectangle.h"

// TODO: move to settings
#define MAP_SCROLL_SPEED 1.0f
#define MAP_ZOOM_SPEED 0.01f

namespace game {
namespace world {

World::World( const Settings& settings )
	: m_settings( settings )
{
	//
}

void World::Start() {
	
	NEW( m_world_scene, Scene, "World", SCENE_TYPE_ORTHO );
	NEW( m_camera, Camera, Camera::CT_ORTHOGRAPHIC );
		m_camera->SetAngle( { M_PI * 0.5, -M_PI * 0.75, 0 } );
	
	m_world_scene->SetCamera( m_camera );
	g_engine->GetGraphics()->AddScene( m_world_scene );	
	
	m_textures.push_back( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 172, 343, 227, 398 ) );
	m_textures.push_back( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 172, 400, 227, 455 ) );
	m_textures.push_back( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 172, 457, 227, 512 ) );
	m_textures.push_back( g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", 172, 514, 227, 569 ) );
	
	// draw some test terrain
	
	const size_t terrain_size = 40;
	const float tile_size = 0.5f;
	const float min_height = 0.0f;
	const float max_height = 1.5f;
	
	const size_t precision = 10000;
	
	const ssize_t s1 = 0;
	const ssize_t s2 = terrain_size;
	const ssize_t x1 = s1;
	const ssize_t y1 = s1;
	const ssize_t x2 = s2;
	const ssize_t y2 = s2;
	
	const float w = tile_size;
	const float h = tile_size;
		
	const size_t heightmap_xsize = x2 - x1 + 2;
	const size_t heightmap_ysize = y2 - y1 + 2;
	float heightmap[ heightmap_ysize ][ heightmap_xsize ] = {};
	
	for ( ssize_t y = y1 ; y < heightmap_ysize ; y++ ) {
		for ( ssize_t x = x1 ; x < heightmap_xsize ; x++ ) {
			heightmap[ x ][ y ] = min_height + (float) ( ( precision * rand() ) % (size_t) ( ( max_height - min_height ) * precision ) ) / precision;
		}
	}
	
	for ( ssize_t y = y1 ; y <= y2 ; y++ ) {
		for ( ssize_t x = x1 ; x <= x2 ; x++ ) {
			
			NEW( m_mesh, mesh::Mesh, 4, 2 );
				m_mesh->AddVertex( { x*w, y*h, heightmap[x][y] }, { 0, 0 } );
				m_mesh->AddVertex( { (x+1)*w, y*h, heightmap[x+1][y] }, { 1, 0 } );
				m_mesh->AddVertex( { (x+1)*w, (y+1)*h, heightmap[x+1][y+1] }, { 1, 1 } );
				m_mesh->AddVertex( { x*w, (y+1)*h, heightmap[x][y+1] }, { 0, 1 } );
				m_mesh->AddSurface( { 2, 3, 0 } );
				m_mesh->AddSurface( { 0, 1, 2 } );
			m_mesh->Finalize();
														
			NEWV( actor, actor::Mesh, "Test", m_mesh );
				actor->SetTexture( m_textures.at( rand() % m_textures.size() ) );
				actor->SetAngle( { 0.0, 0.0, M_PI * 0.25 } );
			m_world_scene->AddActor( actor );
			m_test_actors.push_back( actor );
		}
	}
	
	auto* ui = g_engine->GetUI();
	
	m_handlers.keydown = ui->AddGlobalEventHandler( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( data->key.code == UIEvent::K_ESCAPE ) {
			ReturnToMainMenu();
		}
		return true;
	});
	
	m_handlers.mousedown = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		if ( data->mouse.button == UIEvent::M_MIDDLE ) {
			m_is_dragging = true;
			m_last_drag_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
		}
		return true;
	});
	
	m_handlers.mousemove = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this ) {
		if ( m_is_dragging ) {
			Vec2<float> current_drag_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
			Vec2<float> drag = current_drag_position - m_last_drag_position;
			
			m_camera_position.x += ( (float) drag.x * MAP_SCROLL_SPEED );
			m_camera_position.y += ( (float) drag.y * MAP_SCROLL_SPEED );
			UpdateCameraPosition();
			
			m_last_drag_position = current_drag_position;
		}
		return true;
	});
	
	m_handlers.mouseup = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
		if ( data->mouse.button == UIEvent::M_MIDDLE ) {
			m_is_dragging = false;
		}
		return true;
	});
	
	m_handlers.mousescroll = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_SCROLL, EH( this ) {
		float new_z = m_camera_position.z + (float) data->mouse.y * MAP_ZOOM_SPEED;
		
		if ( new_z < 0.05 ) {
			new_z = 0.05;
		}
		if ( new_z > 1.0 ) {
			new_z = 1.0;
		}
		
		float diff = m_camera_position.z / new_z;
		
		m_camera_position.z = new_z;
		UpdateCameraScale();
		
		m_camera_position.x /= diff;
		m_camera_position.y /= diff;
		UpdateCameraPosition();
		
		return true;
	});
	
	m_clamp.x.SetRange( 0.0, g_engine->GetGraphics()->GetWindowWidth(), -0.5, 0.5 );
	m_clamp.y.SetRange( 0.0, g_engine->GetGraphics()->GetWindowHeight(), -0.5, 0.5 );
	
	SetCameraPosition( { 0, 0, 0.2 } );

	UpdateCameraPosition();
	UpdateCameraScale();
	
	g_engine->GetGraphics()->AddOnResizeHandler( this, RH( this ) {
		m_clamp.x.SetSrcRange( 0.0, g_engine->GetGraphics()->GetWindowWidth() );
		m_clamp.y.SetSrcRange( 0.0, g_engine->GetGraphics()->GetWindowHeight() );
		UpdateCameraPosition();
	});
}

void World::Stop() {
	
	g_engine->GetGraphics()->RemoveOnResizeHandler( this );
	
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.keydown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousedown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousemove );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mouseup );
	
	for ( auto& actor : m_test_actors ) {
		m_world_scene->RemoveActor( actor );
		DELETE( actor );
	}
		
	g_engine->GetGraphics()->RemoveScene( m_world_scene );	
	DELETE( m_world_scene );
	DELETE( m_camera );
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

void World::UpdateCameraPosition() {
	m_camera->SetPosition( { ( 0.5f + m_camera_position.x ) * g_engine->GetGraphics()->GetAspectRatio(), 0.5f + m_camera_position.y, 1 } );
}
void World::UpdateCameraScale() {
	m_camera->SetScale( { m_camera_position.z, m_camera_position.z, m_camera_position.z } );
}

void World::ReturnToMainMenu() {	
	NEWV( task, game::mainmenu::MainMenu );
	g_engine->GetScheduler()->RemoveTask( this );
	g_engine->GetScheduler()->AddTask( task );
}

}
}
