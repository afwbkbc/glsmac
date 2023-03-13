#include "MiniMap.h"

#include "engine/Engine.h"
#include "../../World.h"


namespace game {
namespace world {
namespace ui {

MiniMap::MiniMap( World* world )
	: Section( world, "MiniMap" )
{
	m_config.no_inner_border = true;
}

void MiniMap::Create() {
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
			m_world->CenterAtCoordinatePercents({
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
			if ( m_is_dragging ) {
				f_scrollto({
					data->mouse.relative.x,
					data->mouse.relative.y
				});
			}
			return false;
		});
		
		auto* ui = g_engine->GetUI();
		
		// TODO: make dragging global event
		
		// mousemove should keep working if mouse is outside minimap but dragging continues
		m_handlers.mouse_move = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this, f_scrollto ) {
			if ( m_is_dragging ) {
				auto area = m_map_surface->GetObjectArea();
				f_scrollto({
					data->mouse.absolute.x - (ssize_t)area.left,
					data->mouse.absolute.y - (ssize_t)area.top
				});
			}
			return false;
		}, ::ui::UI::GH_BEFORE );
		
		// dragging should be cancelable anywhere
		m_handlers.mouse_up = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
			if ( m_is_dragging ) {
				m_is_dragging = false;
			}
			return false;
		}, ::ui::UI::GH_BEFORE );
		
		m_map_surface->On( UIEvent::EV_MOUSE_SCROLL, EH( this, f_scrollto ) {
			f_scrollto({
				data->mouse.relative.x,
				data->mouse.relative.y
			});
			m_world->SmoothScroll( data->mouse.scroll_y );
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

	Vec2< ssize_t > size = {
		(ssize_t)round( (float) m_map_surface->GetWidth() * zoom.x ),
		(ssize_t)round( (float) m_map_surface->GetHeight() * zoom.y )
	};
	
	if ( size.x <= 0 || size.y <= 0 ) {
		// prevent crash
		ClearMinimapSelection();
		return;
	}
	
	const size_t w = m_map_surface->GetWidth();
	const size_t h = m_map_surface->GetHeight();
	
	if ( size.x > w ) {
		size.x = w;
	}
	if ( size.y > h ) {
		size.y = h;
	}
	const size_t shw = size.x / 2;
	const size_t shh = size.y / 2;
	
	// to fix oddity
	size.x = shw * 2;
	size.y = shh * 2;
	
	// draw 3 quads with 1 screen distance between them, to make selection also appear from other side when moved to one side
	const Vec2< size_t > full_size = {
		w * 2 + size.x,
		h
	};
	
	Vec2< ssize_t > top_left = {
		(ssize_t)floor( w * position_percents.x - full_size.x / 2 ) + 1,
		(ssize_t)floor( h * position_percents.y - full_size.y / 2 ) + 29
	};
	
	if ( size != m_last_selection_size || !m_map_selection ) {
		// redraw selection texture
		// TODO: add 'lines' shader program and do it without texture
		
		//Log( "Redrawing selection texture ( size=" + size.ToString() + " top_left=" + top_left.ToString() + " )" );
	
		ClearMinimapSelection();
		
		const Color c( 1.0f, 1.0f, 1.0f, 0.6f );

		const Vec2< size_t > half_size = {
			full_size.x / 2,
			full_size.y / 2
		};

		NEW( m_map_selection_texture, types::Texture, "MapSelection", full_size.x, full_size.y );
		for ( auto y = 0 ; y < size.y ; y++ ) {
			const auto yy = half_size.y - shh + y;
			// left instance
			m_map_selection_texture->SetPixel( 0, yy, c );
			m_map_selection_texture->SetPixel( size.x - 1, yy, c );
			// center instance
			m_map_selection_texture->SetPixel( half_size.x - shw, yy, c );
			m_map_selection_texture->SetPixel( half_size.x + shw - 1, yy, c );
			// right instance
			m_map_selection_texture->SetPixel( full_size.x - size.x, yy, c );
			m_map_selection_texture->SetPixel( full_size.x - 1, yy, c );
		}
		for ( auto x = 0 ; x < size.x ; x++ ) {
			// left instance
			m_map_selection_texture->SetPixel( x, half_size.y - shh, c );
			m_map_selection_texture->SetPixel( x, half_size.y + shh - 1, c );
			// center instance
			m_map_selection_texture->SetPixel( x + half_size.x - shw, half_size.y - shh, c );
			m_map_selection_texture->SetPixel( x + half_size.x - shw, half_size.y + shh - 1, c );
			// right instance
			m_map_selection_texture->SetPixel( x + full_size.x - size.x, half_size.y - shh, c );
			m_map_selection_texture->SetPixel( x + full_size.x - size.x, half_size.y + shh - 1, c );
		}
		
		NEW( m_map_selection, object::Mesh );
			m_map_selection->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
			m_map_selection->SetLeft( top_left.x );
			m_map_selection->SetTop( top_left.y );
			m_map_selection->SetWidth( full_size.x );
			m_map_selection->SetHeight( full_size.y );
			m_map_selection->SetMesh( types::mesh::Render::Rectangle() );
			m_map_selection->SetTexture( m_map_selection_texture );
			m_map_selection->SetAreaLimitsByObject( m_map_surface );
		AddChild( m_map_selection );
	
		m_last_selection_size = size;
	}
	else {
		m_map_selection->SetLeft( top_left.x );
		m_map_selection->SetTop( top_left.y );
	}
	
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
	
	m_last_selection_size = { 0, 0 };
}

const bool MiniMap::IsMouseDragging() const {
	return m_is_dragging;
}

void MiniMap::Destroy() {
	
	ClearMinimapSelection();
	
	RemoveChild( m_turn_complete_button );
	
	auto* ui = g_engine->GetUI();
	ui->RemoveGlobalEventHandler( m_handlers.mouse_move );
	ui->RemoveGlobalEventHandler( m_handlers.mouse_up );
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
