#include "Minimap.h"

#include "ui/dom/Widget.h"
#include "types/mesh/Rectangle.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "types/texture/Texture.h"
#include "ui/UI.h"
#include "input/Event.h"
#include "game/frontend/Game.h"
#include "types/texture/Texture.h"

namespace game {
namespace frontend {
namespace widget {

Minimap::Minimap( Game* const game, ui::UI* const ui )
	: Widget( game, ui, ui::WT_MINIMAP, "minimap", {} ) {}

Minimap::~Minimap() {
	if ( m_last_texture ) {
		delete m_last_texture;
	}
}

void Minimap::SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom ) {

	// save for future redraws
	m_last_position_percents = position_percents;
	m_last_zoom = zoom;

	WithWidget(
		F_WITH_WIDGET( this, &position_percents, &zoom ) {
			SetMinimapSelectionImpl( widget, position_percents, zoom );
		}
	);
}

void Minimap::Register( ui::dom::Widget* const widget ) {
	AddMeshAndTexture( widget, LI_PREVIEW );
	AddMeshAndTexture( widget, LI_SELECTION );

	const auto* a = &widget->GetGeometry()->m_area;

	const auto f_scrollto = [ this, a ]( const types::Vec2< ui::coord_t > coords ) -> void {
		//Log( "Minimap select at " + coords.ToString() );
		auto x = coords.x;
		if ( x < 0 ) {
			x = 0;
		}
		if ( x >= a->width ) {
			x = a->width - 1;
		}
		x -= a->width / 2;
		if ( x < 0 ) {
			x += a->width - 4; // TODO: where does this 4 come from?
		}
		m_game->CenterAtCoordinatePercents(
			{
				x / a->width,
				coords.y / a->height
			}
		);
	};

	widget->SetEventHandler(
		input::EV_MOUSE_DOWN, F_EVENT_HANDLER( this, a, f_scrollto ) {
			if ( event.data.mouse.button == input::MB_LEFT ) {
				m_is_dragging = true;
				f_scrollto(
					{
						event.data.mouse.x - a->left,
						event.data.mouse.y - a->top
					}
				);
				return true;
			}
			return false;
		}
	);
	widget->SetGlobalEventHandler(
		input::EV_MOUSE_MOVE, F_EVENT_HANDLER( this, a, f_scrollto ) {
			if ( m_is_dragging ) {
				f_scrollto(
					{
						event.data.mouse.x - a->left,
						event.data.mouse.y - a->top
					}
				);
				return true;
			}
			return false;
		}
	);
	widget->SetGlobalEventHandler(
		input::EV_MOUSE_UP, F_EVENT_HANDLER( this ) {
			if ( event.data.mouse.button == input::MB_LEFT ) {
				if ( m_is_dragging ) {
					m_is_dragging = false;
					return true;
				}
			}
			return false;
		}
	);
	widget->SetEventHandler(
		input::EV_MOUSE_SCROLL, F_EVENT_HANDLER( this, a, f_scrollto ) {
			f_scrollto(
				{
					event.data.mouse.x - a->left,
					event.data.mouse.y - a->top
				}
			);
			m_game->SmoothScroll( event.data.mouse.scroll_y );
			return true;
		}
	);

	if ( m_last_texture ) {
		UpdateImpl( widget, m_last_texture );
		SetMinimapSelectionImpl( widget, m_last_position_percents, m_last_zoom );
	}
}

const types::Vec2< ui::coord_t > Minimap::FindLargestArea() const {
	types::Vec2< ui::coord_t > result = { 0.0f, 0.0f };
	WITH_WIDGET( &result ) {
		const auto& area = widget->GetGeometry()->m_area;
		if ( area.width > result.x ) {
			result.x = area.width;
		}
		if ( area.height > result.y ) {
			result.y = area.height;
		}
	} );
	return result;
}

void Minimap::Update( ui::dom::Widget* const widget, const void* const data ) {
	const auto* const minimap_texture = (const types::texture::Texture*)data;
	const auto w = minimap_texture->GetWidth();
	const auto h = minimap_texture->GetHeight();

	// save last texture for redraws later
	if ( !m_last_texture ) {
		m_last_texture = new types::texture::Texture();
	}
	m_last_texture->Resize( w, h );
	m_last_texture->AddFrom( minimap_texture, types::texture::AM_DEFAULT, 0, 0, w - 1, h - 1 );

	WITH_WIDGET( this, &minimap_texture, &w, &h ) { // TODO: refactor
		UpdateImpl( widget, minimap_texture );
	} );
}

void Minimap::UpdateImpl( ui::dom::Widget* const widget, const types::texture::Texture* const texture ) {
	// TODO: downscale in case of multiple minimaps of different sizes
	auto* const wtexture = widget->GetTexture( LI_PREVIEW );
	const auto w = texture->GetWidth();
	const auto h = texture->GetHeight();
	if ( w == wtexture->GetWidth() && h == wtexture->GetHeight() ) { // update only if matches exactly (won't match for example during resize)
		wtexture->AddFrom( texture, types::texture::AM_MIRROR_Y, 0, 0, w - 1, h - 1 );
		wtexture->FullUpdate();
	}
}

void Minimap::SetMinimapSelectionImpl( ui::dom::Widget* const widget, const types::Vec2< float > position_percents, const types::Vec2< float > zoom ) {
	const size_t w = widget->GetGeometry()->m_area.width;
	const size_t h = widget->GetGeometry()->m_area.height;

	const types::Vec2< ssize_t > ss = { // selection size
		(ssize_t)( w * zoom.x ),
		(ssize_t)( h * zoom.y ),
	};
	if ( ss.x <= 0 || ss.y <= 0 ) {
		return;
	}

	// final rectangle coords
	const types::Vec2< ssize_t > top_left = {
		(ssize_t)( w * position_percents.x - ss.x / 2 ),
		(ssize_t)( h * position_percents.y - ss.y / 2 ),
	};
	const types::Vec2< ssize_t > bottom_right = {
		top_left.x + ss.x,
		top_left.y + ss.y,
	};

	const auto& texture = widget->GetTexture( LI_SELECTION );

	// redraw selection texture
	// TODO: add 'lines' shader program and do it without texture
	// TODO: optimize?

	const types::Color c( 1.0f, 1.0f, 1.0f, 0.6f );

	texture->Resize( w, h );
	texture->Clear();
	for ( char instance = -1 ; instance <= 1 ; instance += 2 ) {
		const auto shift = w / 2 * instance;
		for ( auto y = top_left.y ; y < bottom_right.y ; y++ ) {
			texture->SetPixelMaybe( shift + top_left.x, y, c );
			texture->SetPixelMaybe( shift + bottom_right.x, y, c );
		}
		for ( auto x = top_left.x ; x < bottom_right.x ; x++ ) {
			texture->SetPixelMaybe( shift + x, top_left.y, c );
			texture->SetPixelMaybe( shift + x, bottom_right.y, c );
		}
	}
	texture->FullUpdate();
}

}
}
}
