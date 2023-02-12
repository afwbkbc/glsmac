#pragma once

#include <vector>
#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL.h>
#include <GL/glew.h>

#include "../Graphics.h"

#include "Scene.h"

#include "ui/event/UIEvent.h"

#include "shader_program/ShaderProgram.h"
#include "routine/Routine.h"

using namespace ui;
namespace ui {
using namespace event;
}

namespace graphics {
namespace opengl {

CLASS( OpenGL, Graphics )
	OpenGL( const std::string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const bool fullscreen );
	~OpenGL();
	void Start();
	void Stop();
	void Iterate();
	void AddScene( scene::Scene *scene );
	void RemoveScene( scene::Scene *scene );
	const unsigned short GetViewportWidth() const {
		return m_options.viewport_width;
	}
	const unsigned short GetViewportHeight() const {
		return m_options.viewport_height;
	}
	void LoadTexture( const types::Texture* texture );
	void UnloadTexture( const types::Texture* texture );
	void EnableTexture( const types::Texture* texture );
	void DisableTexture();

	const bool IsFullscreen() const;
	void SetFullscreen();
	void SetWindowed();
	
	const bool IsMouseLocked() const;
	
	void ResizeViewport( const size_t width, const size_t height );
	void ResizeWindow( const size_t width, const size_t height );
	
protected:
	struct {
		std::string title;
		unsigned short viewport_width;
		unsigned short viewport_height;
		bool vsync;
	} m_options;
	struct {
		unsigned short width;
		unsigned short height;
	} m_window_size;
	float m_aspect_ratio;
	SDL_Window *m_window;
	SDL_GLContext m_gl_context;
	std::vector<shader_program::ShaderProgram *> m_shader_programs;
	std::vector<routine::Routine *> m_routines;
	
	void OnResize();
	
private:
	
	typedef std::unordered_map< const types::Texture*, GLuint > m_textures_map;
	m_textures_map m_textures;
	GLuint m_no_texture;
	GLuint next_texture_obj_id = 0;
	
	std::unordered_map< uint8_t, Vec2< ssize_t > > m_active_mousedowns;
	
	bool m_is_fullscreen = false;
};

} /* namespace opengl */
} /* namespace graphics */
