#include "Console.h"

#include "scene/Scene.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "loader/texture/TextureLoader.h"
#include "loader/font/FontLoader.h"
#include "input/Input.h"
#include "input/Event.h"

#include "types/mesh/Rectangle.h"
#include "types/Color.h"
#include "scene/actor/Mesh.h"
#include "scene/actor/Text.h"

static const size_t s_max_history_size = 1024;
static const size_t s_font_size = 15;
static const size_t s_font_padding = 4;
static const types::Color::color_t s_text_color = { 0.5f, 1.0, 0.6f, 1.0f };
static const auto s_ui_flags = scene::actor::Actor::RF_IGNORE_CAMERA | scene::actor::Actor::RF_IGNORE_LIGHTING | scene::actor::Actor::RF_USE_2D_POSITION;

namespace console {

Console::Console() {
	m_graphics = g_engine->GetGraphics();
	m_font = g_engine->GetFontLoader()->GetBuiltinFont( s_font_size );
}

void Console::Start() {
	Log( "Initializing console" );

	m_graphics->AddOnWindowResizeHandler(
		this, [ this ]( const float aspect_ratio ) {
			if ( m_is_enabled ) {
				HideLines();
				Realign();
				ShowLines();
			}
		}
	);

	g_engine->GetInput()->AddHandler(
		this, [ this ]( const input::Event& event ) {
			switch ( event.type ) {
				case input::EV_KEY_DOWN: {
					if ( !event.data.key.modifiers && event.data.key.code == input::K_GRAVE ) {
						if ( !m_is_enabled ) {
							Enable();
						}
						else {
							Disable();
						}
						return true;
					}
/*					if ( m_is_enabled ) {
						// console is open

						if ( !event.data.key.modifiers && event.data.key.code == input::K_ESCAPE ) {
							Disable();
							return true;
						}

						if ( // don't block these
							( event.data.key.modifiers == input::KM_ALT && event.data.key.code == input::K_ENTER )
							) {
							return false;
						}

						return true; // block everything else
					}*/
					break;
				}
				default: {
				}
			}
			return false;
		}
	);

	g_engine->AddLogCallback(
		this, [ this ]( const std::string& text ) {
			AddLine( text );
		}
	);
}

void Console::Stop() {
	Log( "Destroying console" );

	g_engine->RemoveLogCallback( this );
	g_engine->GetInput()->RemoveHandler( this );
	m_graphics->RemoveOnWindowResizeHandler( this );

	Disable();
}

void Console::Iterate() {
	if ( m_is_enabled ) {
		std::lock_guard guard( m_history_mutex );
		if ( !m_history_actor_to_remove.empty() ) {
			for ( const auto& actor : m_history_actor_to_remove ) {
				m_scene->RemoveActor( actor );
				delete actor;
				m_actors.lines.erase( actor );
			}
			m_history_actor_to_remove.clear();
		}
		if ( !m_history_actor_to_add.empty() ) {
			const float lines_shift = m_lines_diff * m_history_actor_to_add.size();
			for ( const auto& actor : m_actors.lines ) {
				actor.first->SetPositionY( actor.first->GetPositionY() + lines_shift );
			}
			float top = -1.0f + m_padding_diff + lines_shift;
			for ( const auto& id : m_history_actor_to_add ) {
				top -= m_lines_diff;
				auto it = m_history.find( id );
				it->second.actor = AddLineActor( it->second.text, top );
				m_actors.lines.insert( { it->second.actor, it->first } );
			}
			m_history_actor_to_add.clear();
		}
	}
}

void Console::Enable() {
	if ( m_is_enabled ) {
		return;
	}

	//Log( "Opening console" );

	m_scene = new scene::Scene( "Console", scene::SCENE_TYPE_UI );

	{
		auto* const mesh = new types::mesh::Rectangle( { -1.0f, -1.0f }, { 1.0f, 1.0f }, 0.0f );
		m_actors.background = new scene::actor::Mesh( "ConsoleBackground", mesh );
		m_actors.background->SetRenderFlags( s_ui_flags );
		m_actors.background->SetTexture( g_engine->GetTextureLoader()->GetColorTexture( { 0.0f, 0.025f, 0.0f, 0.6f } ) );
		m_scene->AddActor( m_actors.background );
	}

	m_graphics->AddScene( m_scene );

	Realign();

	m_is_enabled = true;
	ShowLines();
}

void Console::Disable() {
	if ( !m_is_enabled ) {
		return;
	}

	//Log( "Closing console" );

	m_is_enabled = false;
	HideLines();
	m_scene->RemoveActor( m_actors.background );
	delete m_actors.background;
	m_graphics->RemoveScene( m_scene );
	delete m_scene;
	m_scene = nullptr;
}

void Console::AddLine( const std::string& text ) {
	std::lock_guard guard( m_history_mutex );
	auto it = m_history.insert( { m_next_history_id, { text, nullptr } } ).first;
	if ( m_next_history_id > s_max_history_size ) {
		const auto delete_id = m_next_history_id - s_max_history_size;
		const auto& todelete = m_history.at( delete_id );
		if ( m_is_enabled ) {
			/*
			ASSERT( todelete.actor, "line actor not set" );
			m_scene->RemoveActor( todelete.actor );
			delete todelete.actor;
			m_actors.lines.erase( todelete.actor );*/
			m_history_actor_to_remove.push_back( todelete.actor );
		}
		m_history.erase( delete_id );

	}
	if ( m_is_enabled ) {
		/*for ( const auto& actor : m_actors.lines ) {
			actor.first->SetPositionY( actor.first->GetPositionY() + m_lines_diff );
		}
		it->second.actor = AddLineActor( text, -1.0f );
		m_actors.lines.insert( { it->second.actor, it->first } );*/
		m_history_actor_to_add.push_back( m_next_history_id );
	}
	m_next_history_id++;
}

void Console::Realign() {
	const auto vh = m_graphics->GetViewportHeight();
	m_lines_on_screen = std::floor( ( vh - s_font_padding * 2 ) / s_font_size );
	m_lines_diff = 2.0f / m_lines_on_screen;
	m_padding_diff = 2.0f / ( vh / s_font_padding );
}

scene::actor::Text* const Console::AddLineActor( const std::string& text, const float y ) {
	auto* const actor = new scene::actor::Text( m_font, text );
	actor->SetColor( s_text_color );
	actor->SetRenderFlags( s_ui_flags );
	actor->SetPosition( { -1.0f, y, 0.1f } );
	m_scene->AddActor( actor );
	return actor;
}

void Console::ShowLines() {
	std::lock_guard guard( m_history_mutex );
	ASSERT( m_scene, "scene not set" );
	const size_t lines_to_show = std::min( m_history.size(), m_lines_on_screen );
	float top = -1.0f + m_padding_diff + m_lines_diff * lines_to_show;
	size_t skip = m_history.size() - lines_to_show;
	for ( auto& it : m_history ) {
		if ( skip > 0 ) {
			skip--;
			continue;
		}
		if ( !it.second.actor ) {
			top -= m_lines_diff;
			it.second.actor = AddLineActor( it.second.text, top );
			m_actors.lines.insert( { it.second.actor, it.first } );
		}
	}
}

void Console::HideLines() {
	std::lock_guard guard( m_history_mutex );
	ASSERT( m_scene, "scene not set" );
	for ( const auto& it : m_actors.lines ) {
		ASSERT( m_history.find( it.second ) != m_history.end(), "history line not found" );
		m_scene->RemoveActor( it.first );
		delete it.first;
		m_history.at( it.second ).actor = nullptr;
	}
	m_actors.lines.clear();
	m_history_actor_to_add.clear();
	m_history_actor_to_remove.clear();
}

}
