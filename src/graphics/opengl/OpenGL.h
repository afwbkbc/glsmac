#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#define SDL_MAIN_HANDLED 1
#include <SDL.h>
#include <GL/glew.h>

#include "graphics/Graphics.h"

#include "types/Vec2.h"

namespace graphics {
namespace opengl {

class Scene;
class FBO;

namespace shader_program {
class ShaderProgram;
}

namespace routine {
class Routine;
class Overlay;
}

CLASS( OpenGL, Graphics )

	// maximum instances of same actor, more = faster rendering of same objects
	// includes extra instances from map instancing
	// limited by opengl uniform array size, maximum is around 256 so don't exceed it
	// rendering will be split to multiple draw calls if number of instances is larger
	static constexpr size_t MAX_INSTANCES = 224;

	static constexpr float VIEWPORT_MULTIPLIER = 1.0f; // larger size for internal viewport // TODO

	OpenGL( const std::string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const bool fullscreen );
	~OpenGL();
	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddScene( scene::Scene* scene ) override;
	void RemoveScene( scene::Scene* scene ) override;

	const unsigned short GetWindowWidth() const override;
	const unsigned short GetWindowHeight() const override;
	const unsigned short GetViewportWidth() const override;
	const unsigned short GetViewportHeight() const override;

	void LoadTexture( types::texture::Texture* texture ) override;
	void UnloadTexture( const types::texture::Texture* texture ) override;

	void WithTexture( const types::texture::Texture* texture, const f_t& f ) override;

	FBO* CreateFBO();
	void DestroyFBO( FBO* fbo );

	const bool IsFullscreen() const override;
	void SetFullscreen() override;
	void SetWindowed() override;

	void RedrawOverlay() override;

	const bool IsMouseLocked() const override;

	void ResizeViewport( const size_t width, const size_t height );
	void ResizeWindow( const size_t width, const size_t height ) override;

	void WithBindBuffer( GLenum target, GLuint buffer, const f_t& f ) const;
	void WithBindBuffers( GLuint vbo, GLuint ibo, const f_t& f ) const;
	void WithBindTexture( GLuint texture, const f_t& f ) const;
	void WithBindFramebufferBegin( GLenum target, GLuint buffer ) const;
	void WithBindFramebuffer( GLenum target, GLuint buffer, const f_t& f ) const;
	void WithBindFramebufferEnd( GLenum target ) const;

protected:
	struct {
		std::string title;
		unsigned short viewport_width;
		unsigned short viewport_height;
		bool vsync;
	} m_options;
	types::Vec2< unsigned short > m_window_size;
	types::Vec2< unsigned short > m_last_window_size;
	types::Vec2< unsigned short > m_viewport_size;
	SDL_Window* m_window;
	SDL_GLContext m_gl_context;
	std::vector< shader_program::ShaderProgram* > m_shader_programs;

	std::vector< routine::Routine* > m_routines;
	// some routines are special
	routine::Overlay* m_routine_overlay = nullptr;

	void OnWindowResize() override;

private:

	struct texture_data_t {
		GLuint obj = 0;
		size_t last_texture_update_counter = 0;
	};
	typedef std::unordered_map< const types::texture::Texture*, texture_data_t > m_textures_map;
	m_textures_map m_textures = {};
	GLuint m_no_texture = 0;

	std::unordered_map< uint8_t, types::Vec2< ssize_t > > m_active_mousedowns = {};

	std::unordered_set< FBO* > m_fbos = {};

	bool m_is_fullscreen = false;

	void UpdateViewportSize( const size_t width, const size_t height );
};

}
}
