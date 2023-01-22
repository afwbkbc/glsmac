#include "UI.h"

#include "engine/Engine.h"

#include "event/MouseMove.h"

using namespace scene;
using namespace types;

namespace ui {

using namespace object;

void UI::Start() {
	Log( "Creating UI" );

	NEW( m_shape_scene, Scene, "UIShape", SCENE_TYPE_ORTHO );
	g_engine->GetGraphics()->AddScene( m_shape_scene );

	NEW( m_text_scene, Scene, "UIText", SCENE_TYPE_TEXT );
	g_engine->GetGraphics()->AddScene( m_text_scene );

	m_root_object.Create();
	m_root_object.UpdateObjectArea();

	m_clamp.x.SetRange( 0.0, g_engine->GetGraphics()->GetWindowWidth(), -1.0, 1.0 );
	m_clamp.y.SetRange( 0.0, g_engine->GetGraphics()->GetWindowHeight(), -1.0, 1.0 );
	m_clamp.y.SetInversed( true );
	
#if DEBUG
	NEW( m_debug_scene, Scene, "UIDebug", SCENE_TYPE_ORTHO );
	g_engine->GetGraphics()->AddScene( m_debug_scene );	
#endif
}

void UI::Stop() {
	Log( "Destroying UI" );

#if DEBUG
	g_engine->GetGraphics()->RemoveScene( m_debug_scene );
	DELETE( m_debug_scene );
#endif
	
	g_engine->GetGraphics()->RemoveScene( m_text_scene );
	DELETE( m_text_scene );

	g_engine->GetGraphics()->RemoveScene( m_shape_scene );
	DELETE( m_shape_scene );
	
	m_root_object.Destroy();
}

void UI::AddObject( object::UIObject *object ) {
	m_root_object.AddChild( object );
}
void UI::RemoveObject( object::UIObject *object ) {
	m_root_object.RemoveChild( object );
}

Scene *UI::GetShapeScene() {
	return m_shape_scene;
}

Scene *UI::GetTextScene() {
	return m_text_scene;
}

const UI::coord_t UI::ClampX( const UI::coord_t value ) const {
	return m_clamp.x.Clamp( value );
}

const UI::coord_t UI::ClampY( const UI::coord_t value ) const {
	return m_clamp.y.Clamp( value );
}

void UI::Resize() {
#if DEBUG
	for (auto& it : m_debug_frames) {
		ResizeDebugFrame( it.first, &it.second );
	}
#endif
	
	m_clamp.x.SetSrcRange( 0.0, g_engine->GetGraphics()->GetWindowWidth() );
	m_clamp.y.SetSrcRange( 0.0, g_engine->GetGraphics()->GetWindowHeight() );
	m_root_object.Realign();
}

void UI::Iterate() {
	m_root_object.Iterate();	
}

void UI::ProcessEvent( UIEvent* event ) {
	if ( event->m_type == UIEvent::EV_MOUSE_MOVE ) {
		// need to save last mouse position to be able to trigger mouseover/mouseout events for objects that will move/resize themselves later
		m_last_mouse_position = { event->m_data.mouse.x, event->m_data.mouse.y };
	}
	
	m_root_object.ProcessEvent( event );
}

void UI::SendMouseMoveEvent( UIObject* object ) {
	NEWV( event, MouseMove, m_last_mouse_position.x, m_last_mouse_position.y );
	object->ProcessEvent( event );
	DELETE( event );
}

const UIEventHandler* UI::AddGlobalEventHandler( const UIEvent::event_type_t event_type, const UIEventHandler::handler_function_t& handler ) {
	return m_root_object.On( event_type, handler );
}

void UI::RemoveGlobalEventHandler( const UIEventHandler* event_handler ) {
	m_root_object.Off( event_handler );
}

void UI::SetTheme( theme::Theme* theme ) {
	ASSERT( !m_theme, "theme already set" ); // TODO: make changeable?
	m_theme = theme;
	m_theme->Finalize();
}

void UI::UnsetTheme() {
	ASSERT( m_theme, "theme wasn't set" );
	m_theme = nullptr;
}

const theme::Theme* UI::GetTheme() const {
	return m_theme;
}

const theme::Style* UI::GetStyle( const string& style_class ) const {
	return GetTheme()->GetStyle( style_class );
}

#if DEBUG
void UI::ShowDebugFrame( const UIObject* object ) {
	auto it = m_debug_frames.find( object );
	if ( it == m_debug_frames.end() ) {
		Log("Showing debug frame for " + object->GetName());
		debug_frame_data_t data;
		
		// semi-transparent 1x1 texture with random color for every frame
		NEW( data.texture, Texture, "DebugTexture", 1, 1 );
		data.texture->SetPixel( 0, 0, Color::RGBA( rand() % 256, rand() % 256, rand() % 256, 160 ) );
		
		NEW( data.mesh, mesh::Rectangle );
		
		NEW( data.actor, actor::Mesh, "DebugFrame", data.mesh );
		data.actor->SetTexture( data.texture );
		
		ResizeDebugFrame( object, &data );
		
		m_debug_scene->AddActor( data.actor );
		
		m_debug_frames[object] = data;
	}
}

void UI::HideDebugFrame( const UIObject* object ) {
	auto it = m_debug_frames.find( object );
	if ( it != m_debug_frames.end() ) {
		Log("Hiding debug frame for " + object->GetName());
		m_debug_scene->RemoveActor( it->second.actor );
		DELETE( it->second.actor );
		DELETE( it->second.texture );
		m_debug_frames.erase( it );
	}
}

void UI::ResizeDebugFrame( const UIObject* object, const debug_frame_data_t* data ) {
	auto geom = object->GetAreaGeometry();
	data->mesh->SetCoords({
		ClampX(geom.first.x),
		ClampY(geom.first.y)
	},{
		ClampX(geom.second.x),
		ClampY(geom.second.y)
	}, -1.0 );
	
}

void UI::ResizeDebugFrame( const UIObject* object ) {
	auto it = m_debug_frames.find( object );
	if ( it != m_debug_frames.end() ) {
		ResizeDebugFrame( object, &it->second );
	}
}

#endif

} /* namespace ui */
