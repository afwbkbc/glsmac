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
			Log( "Minimap select at " + coords.ToString() );
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
		m_map_surface->On( UIEvent::EV_MOUSE_MOVE, EH( this, f_scrollto ) {
			if ( m_is_dragging ) {
				f_scrollto( { data->mouse.relative.x, data->mouse.relative.y } );
			}
			return true;
		});
		m_map_surface->On( UIEvent::EV_MOUSE_UP, EH( this ) {
			m_is_dragging = false;
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

void MiniMap::Destroy() {
	RemoveChild( m_turn_complete_button );
	
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
