#include "Text.h"

#include "Container.h"
#include "ui/UI.h"
#include "ui/geometry/Text.h"
#include "scene/Scene.h"
#include "scene/actor/Text.h"
#include "engine/Engine.h"
#include "loader/font/FontLoader.h"
#include "util/String.h"

namespace ui {
namespace dom {

Text::Text( DOM_ARGS )
	: Drawable(
	DOM_ARGS_PASS, "text", new geometry::Text(
		ui, parent
			? parent->GetGeometry()
			: nullptr
	)
) {

	m_actor = new scene::actor::Text( nullptr, "" );
	UpdateFont();
	m_geometry->AsText()->SetActor( m_actor );
	Actor( m_actor );

	Property(
		ctx, call_si, "text", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v ) {
			m_actor->SetText( ( (gse::type::String*)v.Get() )->value );
			m_geometry->SetWidth( m_actor->GetWidth() );
			m_geometry->SetHeight( m_actor->GetHeight() );
		},
		[ this ]( GSE_CALLABLE ) {
			m_actor->SetText( "" );
			m_geometry->SetWidth( 0 );
			m_geometry->SetHeight( 0 );
		}
	);

	Property(
		ctx, call_si, "color", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			types::Color color = {};
			ParseColor( ctx, call_si, ( (gse::type::String*)v.Get() )->value, color );
			m_actor->SetColor( color );
		},
		[ this ]( GSE_CALLABLE ) {
			types::Color color = {};
			m_actor->SetColor( color );
		}
	);

	Property(
		ctx, call_si, "font", gse::type::Type::T_STRING, VALUE( gse::type::String, ":32" ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			const auto parts = util::String::Split( ( (gse::type::String*)v.Get() )->value, ':' );
			if ( parts.size() != 2 ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property 'font' is expected to be font string ('<fontname>:<size>' ), got: " + v.ToString(), ctx, call_si );
			}
			const auto sz = strtoul( parts.at( 1 ).c_str(), nullptr, 10 );
			if ( !sz || sz > 255 ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Invalid font size: " + v.ToString(), ctx, call_si );
			}
			m_fontname = parts.at( 0 );
			m_fontsize = sz;
			UpdateFont();
		},
		[ this ]( GSE_CALLABLE ) {
			m_fontname = 32;
			UpdateFont();
		}
	);

}

Text::~Text() {
}

void Text::UpdateFont() {
	ASSERT_NOLOG( m_fontsize, "font size is zero" );
	ASSERT_NOLOG( m_fontname.empty(), "font loading not supported yet" );
	types::Font* font;
	if ( m_fontname.empty() ) {
		font = g_engine->GetFontLoader()->GetBuiltinFont( m_fontsize );
	}
	else {
		// TODO
	}
	m_actor->SetFont( font );
	m_geometry->SetWidth( m_actor->GetWidth() );
	m_geometry->SetHeight( m_actor->GetHeight() );
}

}
}
