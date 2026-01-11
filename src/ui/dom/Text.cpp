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
		GSE_CALL, "text", gse::VT_STRING, nullptr, PF_NONE, [ this ]( GSE_CALLABLE, gse::Value* const v ) {
			SetText( ( (gse::value::String*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetText( "" );
		}
	);

	Property(
		GSE_CALL, "transform", gse::VT_STRING, nullptr, PF_NONE, [ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto& value = ( (gse::value::String*)v )->value;
			if ( value == "lowercase" ) {
				SetTransform( T_LOWERCASE );
			}
			else if ( value == "uppercase" ) {
				SetTransform( T_UPPERCASE );
			}
			else {
				GSE_ERROR( gse::EC.UI_ERROR, "Unknown transform, expected: none, uppercase or lowercase, got: " + value );
			}
		},
		[ this ]( GSE_CALLABLE ) {
			SetTransform( T_NONE );
		}
	);

	Property(
		GSE_CALL, "color", gse::VT_NOTHING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			types::Color color = {};
			if ( v->type == gse::VT_STRING ) {
				ParseColor( GSE_CALL, ( (gse::value::String*)v )->value, color );
			}
			else if ( v->type == gse::VT_OBJECT && ( (gse::value::Object*)v )->object_class == "Color" ) {
				color = types::Color::Unwrap( v );
			}
			else {
				GSE_ERROR( gse::EC.UI_ERROR, "Property 'color' is expected to be String or Color, got " + v->GetTypeString() + ": " + v->ToString() );
			}
			m_actor->SetColor( color );
		},
		[ this ]( GSE_CALLABLE ) {
			types::Color color = {};
			m_actor->SetColor( color );
		}
	);

	Property(
		GSE_CALL, "font", gse::VT_STRING, VALUE( gse::value::String, , ":32" ), PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto parts = util::String::Split( ( (gse::value::String*)v )->value, ':' );
			if ( parts.size() != 2 ) {
				GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Property 'font' is expected to be font string ('<fontname>:<size>' ), got: " + v->ToString() );
			}
			const auto sz = strtoul( parts.at( 1 ).c_str(), nullptr, 10 );
			if ( !sz || sz > 255 ) {
				GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Invalid font size: " + v->ToString() );
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

void Text::SetText( const std::string& text ) {
	std::string transformed_text = text;
	switch ( m_transform ) {
		case T_LOWERCASE: {
			util::String::ToLowerCase( transformed_text );
			break;
		}
		case T_UPPERCASE: {
			util::String::ToUpperCase( transformed_text );
			break;
		}
		default: {
		}
	}
	if ( transformed_text != m_text ) {
		m_text = transformed_text;
		m_actor->SetText( m_text );
		m_geometry->SetWidth( m_actor->GetWidth() );
		m_geometry->SetHeight( m_actor->GetHeight() );
	}
}

void Text::SetTransform( const transform_t transform ) {
	if ( transform != m_transform ) {
		m_transform = transform;
		SetText( m_text );
	}
}

void Text::UpdateFont() {
	ASSERT( m_fontsize, "font size is zero" );
	types::Font* font;
	if ( m_fontname.empty() ) {
		font = g_engine->GetFontLoader()->GetBuiltinFont( m_fontsize );
	}
	else {
		font = g_engine->GetFontLoader()->LoadCustomFont( m_fontname, m_fontsize );
	}
	m_actor->SetFont( font );
	m_geometry->SetWidth( m_actor->GetWidth() );
	m_geometry->SetHeight( m_actor->GetHeight() );
}

}
}
