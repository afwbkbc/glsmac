#pragma once

#include <vector>
#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "../Graphics.h"

#include "Scene.h"

#include "ui/event/UIEvent.h"

#include "shader_program/OpenGLShaderProgram.h"
#include "routine/OpenGLRoutine.h"

using namespace std;
using namespace ui;
namespace ui {
using namespace event;
}

namespace graphics {
namespace opengl {

MINOR_CLASS( OpenGL, Graphics )
	OpenGLGraphics( const string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const float fov);
	~OpenGLGraphics();
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
	//void UnloadTexture( const types::Texture* texture );
	void EnableTexture( const types::Texture* texture );
	void DisableTexture();

	void ResizeWindow( const size_t width, const size_t height );
	
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
	vector<shader_program::OpenGLShaderProgram *> m_shader_programs;
	vector<routine::OpenGLRoutine *> m_routines;
	struct {
	} m_camera;
	
private:
	void UpdateCamera();
	
	typedef unordered_map< const types::Texture*, GLuint > m_textures_map;
	m_textures_map m_textures;
	GLuint m_no_texture;
	GLuint next_texture_obj_id = 0;
	
	UIEvent::mouse_button_t GetMouseButton( uint8_t sdl_mouse_button ) const;
	UIEvent::key_code_t GetKeyCode( uint8_t sdl_scan_code ) const;
	
	unordered_map< uint8_t, Vec2< ssize_t > > m_active_mousedowns;
	
};

} /* namespace opengl */
} /* namespace graphics */
