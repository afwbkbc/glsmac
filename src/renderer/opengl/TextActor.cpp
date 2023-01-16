#include "TextActor.h"

namespace renderer {
namespace opengl {

TextActor::TextActor( scene::actor::TextActor *actor, Font *gl_font ) : Actor( actor ), m_gl_font( gl_font ) {
	Log( "Creating OpenGL text '" + actor->GetText() + "' with " + gl_font->GetName() + " for " + actor->GetName() );
	auto *text_actor = (const scene::actor::TextActor *)m_actor;
	auto position = m_actor->GetPosition();
	m_gl_font->Update( &m_ctx, text_actor->GetText(), position.x, position.y );
}

TextActor::~TextActor() {
	Log( "Destroying OpenGL text" );
}

void TextActor::Update( const string& text, const float x, const float y ) {
	
	m_gl_font->Update( &m_ctx, text, x, y );
}

void TextActor::Draw( shader_program::OpenGLShaderProgram *shader_program ) {
	
	auto *text_actor = (const scene::actor::TextActor *)m_actor;
	auto position = m_actor->GetPosition();
	m_gl_font->Render( &m_ctx, position.z, text_actor->GetColor() );
}

} /* namespace opengl */
} /* namespace renderer */
