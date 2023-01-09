#include "TextActor.h"

namespace renderer {
namespace opengl {

TextActor::TextActor( scene::actor::TextActor *actor, Font *gl_font ) : Actor( actor ), m_gl_font( gl_font ) {
	Log( "Creating OpenGL text" );
}

TextActor::~TextActor() {
	Log( "Destroying OpenGL text" );
}

void TextActor::Draw( shader_program::OpenGLShaderProgram *shader_program ) {
	const scene::actor::TextActor *text_actor = (const scene::actor::TextActor *)m_actor;
	auto position = m_actor->GetPosition();
	m_gl_font->Render( text_actor->GetText().c_str(), position.x, position.y, position.z, text_actor->GetColor() );
}

} /* namespace opengl */
} /* namespace renderer */
