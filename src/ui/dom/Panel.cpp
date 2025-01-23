#include "Panel.h"

#include "Surface.h"
#include "types/texture/Texture.h"
#include "ui/geometry/Rectangle.h"
#include "util/String.h"

namespace ui {
namespace dom {

Panel::Panel( DOM_ARGS_T, const bool factories_allowed )
	: Container( DOM_ARGS_PASS_T, factories_allowed ) {

	m_surface = new Surface( ctx, call_si, ui, this, {} );
	Embed( m_surface );

	m_border_surface = new Surface( ctx, call_si, ui, this, {} );
	Embed( m_border_surface );

	ForwardProperty( ctx, call_si, "background", m_surface );

	Property(
		ctx, call_si, "border", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			const auto& str = ( (gse::type::String*)v.Get() )->value;
			const auto parts = util::String::Split( str, ',' );
			if ( parts.empty() ) {
				GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Property 'border' expects color code and (optionally) corners" );
			}

			types::Color border_color;
			long int border_corners;
			ParseColor( ctx, call_si, parts.at( 0 ), border_color );
			if ( parts.size() >= 2 ) {
				if ( !util::String::ParseInt( parts.at( 1 ), border_corners ) || border_corners < 0 ) {
					GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Border corners must be numeric and not negative" );
				}
			}
			else {
				border_corners = 0;
			}

			if ( border_color != m_border_color || border_corners != m_border_corners ) {
				m_border_color = border_color;
				m_border_corners = border_corners;
				UpdateBorderTexture();
			}

		},
		[ this ]( GSE_CALLABLE ) {
			m_border_corners = 0;
			m_border_color = types::Color{};
			m_border_surface->ClearTexture();
		}
	);
	UpdateBorderTexture();
	m_geometry->m_on_area_update = [ this ]( const geometry::Geometry::area_t& area ) {
		UpdateBorderTexture();
	};
}

void Panel::UpdateBorderTexture() {
	m_surface->GetGeometry()->SetPadding( m_border_corners / 2 );
	auto* texture = m_border_surface->GetOwnedTexturePtr();
	const auto& a = m_geometry->AsRectangle()->m_area;
	texture->Resize( a.width, a.height );
	memset( texture->m_bitmap, 0, texture->m_bitmap_size );
	if ( m_border_color != types::Color{} ) {
		long int r = std::ceil( std::fmin( m_border_corners, std::min( a.width, a.height ) / 2 - 1 ) );
		for ( auto x = r ; x < a.width - r ; x++ ) {
			texture->SetPixel( x, 0, m_border_color );
			texture->SetPixel( x, a.height - 1, m_border_color );
		}
		for ( auto y = r ; y < a.height - r ; y++ ) {
			texture->SetPixel( 0, y, m_border_color );
			texture->SetPixel( a.width - 1, y, m_border_color );
		}
		for ( auto i = 0 ; i < r ; i++ ) {
			texture->SetPixel( i, r - i, m_border_color );
			texture->SetPixel( r - i, i, m_border_color );
			texture->SetPixel( a.width - 1 - i, r - i, m_border_color );
			texture->SetPixel( a.width - 1 - ( r - i ), i, m_border_color );
			texture->SetPixel( a.width - 1 - i, a.height - 1 - ( r - i ), m_border_color );
			texture->SetPixel( a.width - 1 - ( r - i ), a.height - 1 - i, m_border_color );
			texture->SetPixel( i, a.height - 1 - ( r - i ), m_border_color );
			texture->SetPixel( r - i, a.height - 1 - i, m_border_color );
		}
	}
	texture->FullUpdate(); // TODO: optimize?
}

}
}
