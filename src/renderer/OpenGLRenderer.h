#pragma once

#include <vector>
#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL.h>
#include <GL/glew.h>

#include "Renderer.h"

#include "opengl/Scene.h"

#include "opengl/shader_program/OpenGLShaderProgram.h"
#include "opengl/routine/OpenGLRoutine.h"

using namespace std;

namespace renderer {

MINOR_CLASS( OpenGL, Renderer )
	OpenGLRenderer( const string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const float fov);
	~OpenGLRenderer();
	void Start();
	void Stop();
	void Iterate();
	void AddScene( scene::Scene *scene );
	void RemoveScene( scene::Scene *scene );
	const unsigned short GetWindowWidth() const {
		return m_options.window_width;
	}
	const unsigned short GetWindowHeight() const {
		return m_options.window_height;
	}
	void LoadTexture( const types::Texture* texture );
	void UnloadTexture( const types::Texture* texture );
	void EnableTexture( const types::Texture* texture );
	void DisableTexture();

protected:
	struct {
		string title;
		unsigned short window_width;
		unsigned short window_height;
		bool vsync;
		float fov;
		float znear;
		float zfar;
	} m_options;
	float m_aspect_ratio;
	SDL_Window *m_window;
	SDL_GLContext m_gl_context;
	vector<opengl::shader_program::OpenGLShaderProgram *> m_shader_programs;
	vector<opengl::routine::OpenGLRoutine *> m_routines;
	struct {
	} m_camera;
	
private:
	void UpdateCamera();
	
	unordered_map< const types::Texture*, GLuint > m_textures;
	GLuint next_texture_obj_id = 0;
};

} /* namespace renderer */
