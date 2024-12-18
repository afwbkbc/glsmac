#include "UI.h"

#include "dom/Root.h"

#include "scene/Scene.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"

namespace ui {

UI::UI( GSE_CALLABLE )
	: m_scene(new scene::Scene( "Scene::UI", scene::SCENE_TYPE_UI ) ){

	m_clamp.x.SetRange(
		{
			{ 0.0,  (float)g_engine->GetGraphics()->GetViewportWidth() },
			{ -1.0, 1.0 }
		}
	);
	m_clamp.x.SetOverflowAllowed( true );

	m_clamp.y.SetRange(
		{
			{ 0.0,  (float)g_engine->GetGraphics()->GetViewportHeight() },
			{ -1.0, 1.0 }
		}
	);
	m_clamp.y.SetOverflowAllowed( true );
	m_clamp.y.SetInversed( true );

	g_engine->GetGraphics()->AddOnWindowResizeHandler(
		this, RH( this ) {
			Resize();
		}
	);

	g_engine->GetGraphics()->AddScene( m_scene );
	m_root = new dom::Root( ctx, call_si, this );
	Resize();
}

UI::~UI() {

	delete m_root;

	g_engine->GetGraphics()->RemoveScene( m_scene );
	DELETE( m_scene );

	g_engine->GetGraphics()->RemoveOnWindowResizeHandler( this );

}

WRAPIMPL_BEGIN( UI )
	WRAPIMPL_PROPS
			{
				"root",
				m_root->Wrap( true )
			},
		};
WRAPIMPL_END_PTR( GLSMAC )

void UI::Resize() {
	const auto& g = g_engine->GetGraphics();
/*#ifdef DEBUG
			for ( auto& it : m_debug_frames ) {
				ResizeDebugFrame( it.first, &it.second );
			}
#endif*/
	m_clamp.x.SetSrcRange(
		{
			0.0,
			(float)g->GetViewportWidth()
		}
	);
	m_clamp.y.SetSrcRange(
		{
			0.0,
			(float)g->GetViewportHeight()
		}
	);
	m_root->Resize( g->GetViewportWidth(), g->GetViewportHeight() );

}

const types::Vec2< types::mesh::coord_t > UI::Clamp( const types::Vec2< coord_t >& xy ) const {
	return {
		m_clamp.x.Clamp( xy.x ),
		m_clamp.y.Clamp( xy.y ),
	};
}

}
