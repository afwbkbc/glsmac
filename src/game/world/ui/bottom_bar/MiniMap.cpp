#include "MiniMap.h"

#include "engine/Engine.h"
#include "../../World.h"


namespace game {
namespace world {
namespace ui {

MiniMap::MiniMap( World* world )
	: Section( world, "MiniMap" )
{
	
}

void MiniMap::Create() {
	m_config.no_inner_border = true;
	Section::Create();

	NEW( m_turn_complete_button, object::Button, "MapBottomBarMinimapTurnCompleteButton" );
		m_turn_complete_button->SetLabel( "TURN COMPLETE" );
	AddChild( m_turn_complete_button );
	
	NEW( m_map_surface, object::Mesh, "MapBottomBarMinimapImage" );
		m_map_surface->SetMesh( types::mesh::Render::Rectangle() );
		if ( m_texture ) {
			m_map_surface->SetTexture( m_texture );
		}
		m_map_surface->AddEventContexts( EC_MOUSEMOVE );
		const auto f_scrollto = [ this ] ( const Vec2< ssize_t > coords ) -> void {
			//Log( "Minimap select at " + coords.ToString() );
			m_world->ScrollToCoordinatePercents({
				(float) coords.x / m_map_surface->GetWidth(),
				(float) coords.y / m_map_surface->GetHeight()
			});
		};
		m_map_surface->On( UIEvent::EV_MOUSE_DOWN, EH( this, f_scrollto ) {
			m_is_dragging = true;
			f_scrollto( { data->mouse.relative.x, data->mouse.relative.y } );
			return true;
		});
		m_map_surface->On( UIEvent::EV_MOUSE_OVER, EH( this, f_scrollto ) {
			m_is_mouse_over = true;
			return false;
		});
		m_map_surface->On( UIEvent::EV_MOUSE_OUT, EH( this, f_scrollto ) {
			m_is_mouse_over = false;
			return false;
		});
		m_map_surface->On( UIEvent::EV_MOUSE_MOVE, EH( this, f_scrollto ) {
			m_last_mouse_position = {
				data->mouse.relative.x,
				data->mouse.relative.y
			};
			if ( m_is_dragging ) {
				f_scrollto( m_last_mouse_position );
			}
			return false;
		});
		m_mouseup_handler = g_engine->GetUI()->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) { // mouseup should work anywhere
			if ( m_is_dragging ) {
				m_is_dragging = false;
			}
			return false;
		}, ::ui::UI::GH_BEFORE );
		m_map_surface->On( UIEvent::EV_MOUSE_SCROLL, EH( this, f_scrollto ) { // TODO: fix ui to actually receive this event here
			f_scrollto( m_last_mouse_position );
			m_world->MouseScroll( { 0.5f, 0.5f }, data->mouse.scroll_y );
			return true;
		});

	AddChild( m_map_surface );

	NEW( m_bottom_bar, object::Section, "MapBottomBarMinimapBottomBar" );
	AddChild( m_bottom_bar );
	NEW( m_bottom_bar_labels.mission_year, object::Label, "MapBottomBarMinimapBottomBarLabel" );
		m_bottom_bar_labels.mission_year->SetText( "Mission Year 2101" );
		m_bottom_bar_labels.mission_year->SetTop( 3 );
	m_bottom_bar->AddChild( m_bottom_bar_labels.mission_year );
	NEW( m_bottom_bar_labels.energy, object::Label, "MapBottomBarMinimapBottomBarLabel" );
		m_bottom_bar_labels.energy->SetText( "Energy: 0" );
		m_bottom_bar_labels.energy->SetTop( 18 );
	m_bottom_bar->AddChild( m_bottom_bar_labels.energy );

}

void MiniMap::SetMinimapSelection( const Vec2< float > position_percents, const Vec2< float > zoom ) {
	ClearMinimapSelection();
	
	//Log( "Setting minimap selection to " + position_percents.ToString() + " ( zoom " + zoom.ToString() + " )" );
	
	Vec2< size_t > size = {
		(size_t)floor( (float) m_map_surface->GetWidth() * zoom.x ),
		(size_t)floor( (float) m_map_surface->GetHeight() * zoom.y )
	};
	
	Vec2< ssize_t > top_left = {
		(ssize_t)round( (float) m_map_surface->GetWidth() * position_percents.x - size.x / 2 ),
		(ssize_t)round( (float) m_map_surface->GetHeight() * position_percents.y - size.y / 2 + 29 )
	};
	
	//Log( "Selection size=" + size.ToString() + " top_left=" + top_left.ToString() );
	
	const Color c( 1.0f, 1.0f, 1.0f, 0.5f );
	
	NEW( m_map_selection_texture, types::Texture, "MapSelection", size.x, size.y );
	for ( auto y = 0 ; y < size.y ; y++ ) {
		m_map_selection_texture->SetPixel( 0, y, c );
		m_map_selection_texture->SetPixel( size.x - 1, y, c );
	}
	for ( auto x = 0 ; x < size.x ; x++ ) {
		m_map_selection_texture->SetPixel( x, 0, c );
		m_map_selection_texture->SetPixel( x, size.y - 1, c );
	}
	NEW( m_map_selection, object::Mesh ); //, "MapBottomBarMinimapSelection" );
		m_map_selection->SetWidth( size.x );
		m_map_selection->SetHeight( size.y );
		m_map_selection->SetLeft( top_left.x );
		m_map_selection->SetTop( top_left.y );
		m_map_selection->SetMesh( types::mesh::Render::Rectangle() );
		m_map_selection->SetTexture( m_map_selection_texture );
	AddChild( m_map_selection );
}

void MiniMap::ClearMinimapSelection() {
	if ( m_map_selection ) {
		RemoveChild( m_map_selection );
		m_map_selection = nullptr;
	}
	
	if ( m_map_selection_texture ) {
		DELETE( m_map_selection_texture );
		m_map_selection_texture = nullptr;
	}
}

const bool MiniMap::IsMouseOver() const {
	if ( m_is_mouse_over ) {
		// TODO: fix mousescroll propagation
		m_world->ScrollToCoordinatePercents({
			(float) m_last_mouse_position.x / m_map_surface->GetWidth(),
			(float) m_last_mouse_position.y / m_map_surface->GetHeight()
		});
	}
	return m_is_mouse_over;
}

void MiniMap::Destroy() {
	
	ClearMinimapSelection();
	
	RemoveChild( m_turn_complete_button );
	
	g_engine->GetUI()->RemoveGlobalEventHandler( m_mouseup_handler );
	RemoveChild( m_map_surface );
	
	m_bottom_bar->RemoveChild( m_bottom_bar_labels.mission_year );
	m_bottom_bar->RemoveChild( m_bottom_bar_labels.energy );
	RemoveChild( m_bottom_bar );
	
	Section::Destroy();
}

void MiniMap::SetMinimapTexture( types::Texture* texture ) {
	m_texture = texture;
	if ( m_map_surface ) {
		m_map_surface->SetTexture( m_texture );
	}
}

}
}
}
