#include "Font.h"

#include "../actor/Text.h"

namespace graphics {
namespace opengl {
namespace routine {

Font::Font( OpenGL* opengl, shader_program::Font *shader_program )
	: Routine( opengl )
	, m_shader_program( shader_program )
{
	//
}

void Font::Start() {

}

void Font::Stop() {
	for ( auto it = m_gl_fonts.begin() ; it < m_gl_fonts.end() ; ++it )
		RemoveFont( *it );
	m_gl_fonts.clear();
}

void Font::RemoveFont( base::ObjectLink *link ) {
	DELETE( link->GetDstObject<Font>() );
	DELETE( link );
}

void Font::Iterate() {
	
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

	//glEnable(GL_TEXTURE_2D);

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		(*it)->Draw( m_shader_program );
		ASSERT( !glGetError(), "Font draw error" );
	}

	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);

}

bool Font::SceneBelongs( const scene::Scene *scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_TEXT;
}

opengl::Actor *Font::AddCustomActor( scene::actor::Actor *actor ) {
	auto actor_type = actor->GetType();
	switch (actor_type) {
		case scene::actor::Actor::TYPE_TEXT: {
			auto *text_actor = (scene::actor::Text *)actor;
			auto *font = text_actor->GetFont();
			//base::ObjectLink *font_link = font->m_graphics_object;
			//Font *gl_font;
			//if ( font_link != NULL )
				//gl_font = font_link->GetDstObject<Font>();
			//else {
				//m_shader_program->Enable();
				//NEW( gl_font, Font, font, m_shader_program );
				//m_shader_program->Disable();
				//NEW( font_link, base::ObjectLink, font, gl_font );
				//m_gl_fonts.push_back( font_link );
				//font->m_graphics_object = font_link;
			//}
			NEWV( result, Text, (scene::actor::Text *)actor, font );
			return result;
		}
		default: {
			ASSERT( false, "unknown actor type " + std::to_string( actor_type ) );
		}
	}
	return NULL;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
