#include "Panel.h"

#include "Surface.h"
#include "types/texture/Texture.h"
#include "ui/geometry/Rectangle.h"
#include "util/String.h"
#include "scene/actor/Cache.h"

namespace ui {
namespace dom {

Panel::Panel( DOM_ARGS_T, const bool factories_allowed )
	: Container( DOM_ARGS_PASS_T, factories_allowed, true ) {

	m_surface = new Surface( GSE_CALL, ui, this, {} );
	Embed( m_surface );

	m_border_surface = new Surface( GSE_CALL, ui, this, {} );
	Embed( m_border_surface );

	ForwardProperty( GSE_CALL, "background", m_surface );

	Property(
		GSE_CALL, "border", gse::VT_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto& str = ( (gse::value::String*)v )->value;

			// TODO: refactor
			auto commapos = str.rfind( ',' );
			auto bracketclosepos = str.find( ')' );
			if ( bracketclosepos != std::string::npos && commapos < bracketclosepos ) {
				commapos = std::string::npos;
			}

			types::Color border_color;
			long int border_corners;
			if ( commapos != std::string::npos ) {
				ParseColor( GSE_CALL, str.substr( 0, commapos ), border_color );
				if ( !util::String::ParseInt( str.substr( commapos + 1 ), border_corners ) || border_corners < 0 ) {
					GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Border corners must be numeric and not negative" );
				}
			}
			else {
				ParseColor( GSE_CALL, str, border_color );
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
	GeometryHandler(
		GH_ON_AREA_UPDATE, [ this ]() {
			UpdateBorderTexture();
		}
	);
}

void Panel::UpdateBorderTexture() {
	m_surface->GetGeometry()->SetPadding( std::ceil( m_border_corners / 2 ) );
	auto* texture = m_border_surface->GetOwnedTexturePtr();
	const auto& a = m_geometry->AsRectangle()->m_area;
	texture->Resize( std::round( a.width ), std::round( a.height ) );
	auto* bitmap = texture->GetBitmap();
	if ( bitmap ) {
		memset( bitmap, 0, texture->GetBitmapSize() );
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
		m_cache->Update();
	}
}

}
}
