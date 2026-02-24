#include "Minimap.h"

#include "ui/dom/Widget.h"
#include "types/mesh/Rectangle.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "types/texture/Texture.h"
#include "ui/UI.h"
#include "input/Event.h"
#include "game/frontend/Game.h"

namespace game {
namespace frontend {
namespace widget {

Minimap::Minimap( Game* const game, ui::UI* const ui )
	: Widget( game, ui, ui::WT_MINIMAP, "minimap", {} ) {}

void Minimap::SetMinimapSelection( const types::Vec2< float > position_percents, const types::Vec2< float > zoom ) {

	WithWidget(
		F_WITH_WIDGET( &position_percents, &zoom ) {

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
			m_is_dragging = true;
			f_scrollto(
				{
					event.data.mouse.x - a->left,
					event.data.mouse.y - a->top
				}
			);
			return true;
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
			}
			return false;
		}
	);
	widget->SetGlobalEventHandler(
		input::EV_MOUSE_UP, F_EVENT_HANDLER( this ) {
			if ( m_is_dragging ) {
				m_is_dragging = false;
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
	WITH_WIDGET( &minimap_texture ) {
		// TODO: downscale in case of multiple minimaps of different sizes
		auto* const texture = widget->GetTexture( LI_PREVIEW );
		const auto w = minimap_texture->GetWidth();
		const auto h = minimap_texture->GetHeight();
		if ( w == texture->GetWidth() && h == texture->GetHeight() ) { // update only if matches exactly (won't match for example during resize)
			texture->AddFrom( minimap_texture, types::texture::AM_MIRROR_Y, 0, 0, w - 1, h - 1 );
			texture->FullUpdate();
		}
	} );
}

}
}
}
