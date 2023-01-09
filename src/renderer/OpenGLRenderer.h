#pragma once

#include <vector>

#define SDL_MAIN_HANDLED 1
#include <SDL.h>
#include <GL/glew.h>

#include "Renderer.h"

#include "opengl/Scene.h"

#include "opengl/shader_program/OpenGLShaderProgram.h"
#include "opengl/routine/OpenGLRoutine.h"

namespace renderer {

MINOR_CLASS( OpenGL, Renderer )
	OpenGLRenderer( const std::string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const float fov);
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

protected:
	struct {
		std::string title;
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
	std::vector<opengl::shader_program::OpenGLShaderProgram *> m_shader_programs;
	std::vector<opengl::routine::OpenGLRoutine *> m_routines;
	struct {
	} m_camera;
private:
	void UpdateCamera();
};

} /* namespace renderer */
