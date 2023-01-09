#include "FontOpenGLRoutine.h"

#include "../TextActor.h"

namespace renderer {
namespace opengl {
namespace routine {

void FontOpenGLRoutine::Start() {

}

void FontOpenGLRoutine::Stop() {
	for ( auto it = m_gl_fonts.begin() ; it < m_gl_fonts.end() ; ++it )
		RemoveFont( *it );
	m_gl_fonts.clear();
}

void FontOpenGLRoutine::RemoveFont( base::ObjectLink *link ) {
	delete link->GetDstObject<Font>();
	delete link;
}

void FontOpenGLRoutine::Iterate() {
	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it )
		(*it)->Update();

	// remove missing fonts
	for ( auto it = m_gl_fonts.begin() ; it < m_gl_fonts.end() ; ++it ) {
		if ( (*it)->Removed() ) {
			RemoveFont( *it );
			m_gl_fonts.erase( it, it + 1 );
			it--;
		}
	}

	//glEnable( GL_BLEND );
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it )
		(*it)->Draw( m_shader_program );

	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);

}

bool FontOpenGLRoutine::SceneBelongs( const scene::Scene *scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_TEXT;
}

opengl::Actor *FontOpenGLRoutine::AddCustomActor( scene::actor::Actor *actor ) {
	auto actor_type = actor->GetType();
	switch (actor_type) {
		case scene::actor::Actor::TYPE_TEXT: {
			scene::actor::TextActor *text_actor = (scene::actor::TextActor *)actor;
			types::Font *font = text_actor->GetFont();
			base::ObjectLink *font_link = font->m_renderer_object;
			Font *gl_font;
			if ( font_link != NULL )
				gl_font = font_link->GetDstObject<Font>();
			else {
				m_shader_program->Enable();
				gl_font = new Font( font, m_shader_program );
				m_shader_program->Disable();
				font_link = new base::ObjectLink( font, gl_font );
				m_gl_fonts.push_back( font_link );
				font->m_renderer_object = font_link;
			}
			return new TextActor( (scene::actor::TextActor *)actor, gl_font );
		}
	}
	return NULL;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
