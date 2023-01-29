#include "World.h"
#include "engine/Engine.h"

#include "../mainmenu/MainMenu.h"

#include "scene/mesh/Rectangle.h"

#include "map_generator/SimpleRandom.h"
#include "map_generator/SimplePerlin.h"
#include "map_generator/Test.h"

// TODO: move to settings
#define MAP_SCROLL_SPEED 1.0f
#define MAP_ROTATE_SPEED 2.0f
#define MAP_ZOOM_SPEED 0.1f

#define INITIAL_CAMERA_ANGLE { -M_PI * 0.5, M_PI * 0.75, 0 }

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
	
	m_camera_angle = INITIAL_CAMERA_ANGLE;
	UpdateCameraAngle();
	
	m_world_scene->SetCamera( m_camera );
	g_engine->GetGraphics()->AddScene( m_world_scene );	
	
	NEW( m_map, Map, m_world_scene );
	
	NEWV( tiles, Tiles, 50, 50 );
	//NEWV( tiles, Tiles, 160, 160 );
	//NEWV( tiles, Tiles, 10, 10 ); // tmp
	
	{
		map_generator::SimplePerlin generator;
		//map_generator::SimpleRandom generator;
		//map_generator::Test generator;
		generator.Generate( tiles );
	}
	
	m_map->SetTiles( tiles );
	
	auto* ui = g_engine->GetUI();
	
	m_handlers.keydown = ui->AddGlobalEventHandler( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( data->key.code == UIEvent::K_ESCAPE ) {
			ReturnToMainMenu();
		}
		return true;
	});
	
	m_handlers.mousedown = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_RIGHT: {
				m_is_dragging = true;
				m_last_drag_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
				break;
			}
			case UIEvent::M_MIDDLE: {
				m_is_rotating = true;
				m_last_rotate_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
				break;
			}
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
		if ( m_is_rotating ) {
			Vec2<float> current_rotate_position = { m_clamp.x.Clamp( data->mouse.x ), m_clamp.y.Clamp( data->mouse.y ) };
			Vec2<float> rotate = current_rotate_position - m_last_rotate_position;
			
			auto* actor = m_map->GetActor();
			auto newz = actor->GetAngleZ() + ( (float) rotate.x * MAP_ROTATE_SPEED );
			auto newy = max( -0.5f, min( 0.5f, actor->GetAngleY() + ( (float) rotate.y * MAP_ROTATE_SPEED ) ) );
			actor->SetAngleZ( newz );
			actor->SetAngleY( newy );
			m_last_rotate_position = current_rotate_position;
		}
		
		return true;
	});
	
	m_handlers.mouseup = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_RIGHT: {
				m_is_dragging = false;
				break;
			}
			case UIEvent::M_MIDDLE: {
				m_is_rotating = false;
				m_map->GetActor()->SetAngle( { 0.0, 0.0, 0.0 } );
				break;
			}
		}
		return true;
	});
	
	m_handlers.mousescroll = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_SCROLL, EH( this ) {
		
		float speed = (float) MAP_ZOOM_SPEED * m_camera_position.z;
		
		float new_z = m_camera_position.z + (float) data->mouse.scroll_y * speed;
		
		if ( new_z < 0.05 ) {
			new_z = 0.05;
		}
		if ( new_z > 0.4 ) {
			new_z = 0.4;
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
	
	SetCameraPosition( { 0.0f, 0.0f, 0.05f } );

	UpdateCameraPosition();
	UpdateCameraScale();
	
	g_engine->GetGraphics()->AddOnResizeHandler( this, RH( this ) {
		m_clamp.x.SetSrcRange( 0.0, g_engine->GetGraphics()->GetWindowWidth() );
		m_clamp.y.SetSrcRange( 0.0, g_engine->GetGraphics()->GetWindowHeight() );
		UpdateCameraPosition();
	});
}

void World::Stop() {
	
	DELETE( m_map );
	
	g_engine->GetGraphics()->RemoveOnResizeHandler( this );
	
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.keydown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousedown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousemove );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mouseup );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousescroll );
	
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
	m_camera->SetPosition( { ( 0.5f + m_camera_position.x ) * g_engine->GetGraphics()->GetAspectRatio(), 0.5f + m_camera_position.y, 1.0f + m_camera_position.y } );
}
void World::UpdateCameraScale() {
	m_camera->SetScale( { m_camera_position.z, m_camera_position.z, m_camera_position.z } );
}

void World::UpdateCameraAngle() {
	m_camera->SetAngle( m_camera_angle );
}

void World::ReturnToMainMenu() {	
	
	//g_engine->ShutDown(); // TMP
	
	NEWV( task, game::mainmenu::MainMenu );
	g_engine->GetScheduler()->RemoveTask( this );
	g_engine->GetScheduler()->AddTask( task );
	
}

}
}
