#include "Text.h"

#include "ui/UI.h"
#include "scene/Scene.h"
#include "scene/actor/Text.h"

#include "engine/Engine.h"
#include "loader/font/FontLoader.h"

namespace ui {
namespace dom {

Text::Text( DOM_ARGS )
	: Object( DOM_ARGS_PASS, "text" ) {

	const auto color = types::Color::FromRGB( 255, 0, 255 );
	auto* font = g_engine->GetFontLoader()->GetBuiltinFont( 32 );
	NEW( m_actor, scene::actor::Text, font, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz", color );
	m_actor->SetPosition(
		{
			-0.84f,
			0.25f,
			0.7f
		}
	);
	m_ui->m_scene->AddActor( m_actor );

	Property(
		ctx, call_si, "text", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v ) {
			// TODO
		}
	);

	Property(
		ctx, call_si, "color", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE, [ this ]( GSE_CALLABLE, const gse::Value& v ) {
			types::Color color;
			ParseColor( ctx, call_si, ( (gse::type::String*)v.Get() )->value, color );
			// TODO
		}
	);

}

Text::~Text() {
	if ( m_actor ) {
		m_ui->m_scene->RemoveActor( m_actor );
		delete m_actor;
	}
}

}
}
