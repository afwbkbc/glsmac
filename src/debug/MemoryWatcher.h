#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <GL/glew.h>

namespace base {
class Base;
}

using namespace base;

namespace debug {

class MemoryWatcher {
public:
	MemoryWatcher( const bool memory_debug = false );

	~MemoryWatcher();

	// memory stuff
	void New( const void* object, const size_t size, const std::string& file, const size_t line );

	void Delete( const void* object, const std::string& file, const size_t line );

	void* Malloc( const size_t size, const std::string& file, const size_t line );

	void* Realloc( void* ptr, const size_t size, const std::string& file, const size_t line );

	unsigned char* Ptr( unsigned char* ptr, const size_t offset, const size_t size, const std::string& file, const size_t line );

	void Free( void* ptr, const std::string& file, const size_t line );

	// opengl stuff
	void GLGenBuffers( GLsizei n, GLuint* buffers, const std::string& file, const size_t line );

	void GLBindBuffer( GLenum target, GLuint buffer, const std::string& file, const size_t line );

	void GLBufferData( GLenum target, GLsizeiptr size, const void* data, GLenum usage, const std::string& file, const size_t line );

	void GLDeleteBuffers( GLsizei n, const GLuint* buffers, const std::string& file, const size_t line );

	void GLGenTextures( GLsizei n, GLuint* textures, const std::string& file, const size_t line );

	void GLBindTexture( GLenum target, GLuint texture, const std::string& file, const size_t line );

	void GLTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels, const std::string& file, const size_t line );

	void GLTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels, const std::string& file, const size_t line );

	void GLDeleteTextures( GLsizei n, GLuint* textures, const std::string& file, const size_t line );

	void GLGenFramebuffers( GLsizei n, GLuint* buffers, const std::string& file, const size_t line );

	void GLBindFramebuffer( GLenum target, GLuint buffer, const std::string& file, const size_t line );

	void GLFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, const std::string& file, const size_t line );

	void GLDeleteFramebuffers( GLsizei n, const GLuint* buffers, const std::string& file, const size_t line );

	GLuint GLCreateProgram( const std::string& file, const size_t line );

	void GLLinkProgram( GLuint program, const std::string& file, const size_t line );

	void GLValidateProgram( GLuint program, const std::string& file, const size_t line );

	void GLUseProgram( GLuint program, const std::string& file, const size_t line );

	void GLDeleteProgram( GLuint program, const std::string& file, const size_t line );

	void GLDrawElements( GLenum mode, GLsizei count, GLenum type, const void* indices, const std::string& file, const size_t line );

	void GLDrawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount, const std::string& file, const size_t line );

	void GLDrawArrays( GLenum mode, GLint first, GLsizei count, const std::string& file, const size_t line );

	struct statistics_item_t {
		size_t size;
		size_t count;
		std::string key;
	};

	typedef std::unordered_map< std::string, statistics_item_t > statistics_t;

	typedef std::vector< statistics_item_t > statistics_result_t;

	const statistics_result_t GetLargestMemoryConsumerClasses( size_t count = 10 );

private:
	const bool m_memory_debug = false;

	std::mutex m_mutex;

	void Log( const std::string& text );

	struct object_info_t {
		const void* ptr = nullptr;
		size_t size = 0;
		std::string object_name = "";
		std::string object_namespace = "";
		std::string source = "";
	};

	std::unordered_map< const void*, object_info_t > m_allocated_objects = {};

	struct alloc_t {
		size_t size = 0;
		std::string source = "";
	};

	std::unordered_map< const void*, alloc_t > m_allocated_memory = {};

	enum framebuffer_mode_t {
		FM_NONE,
		FM_BIND,
		FM_DRAW,
		FM_READ
	};
	struct framebuffer_t {
		struct {
			GLuint render = 0;
			GLuint depth = 0;
		} textures = {};
		std::unordered_set< GLuint > generated_buffers = {}; // buffers generated while framebuffer is bound
		std::unordered_set< GLuint > generated_textures = {}; // textures generated while framebuffer is bound
	};

	struct program_t {
		bool is_linked = false;
		bool is_validated = false;
	};

	struct {
		GLuint current_vertex_buffer = 0;
		GLuint current_index_buffer = 0;
		GLuint current_texture = 0;
		GLuint current_framebuffer = 0;
		GLuint current_program = 0;
		framebuffer_mode_t current_framebuffer_mode = FM_NONE;
		std::unordered_set< GLuint > buffers = {};
		std::unordered_map< GLuint, alloc_t > vertex_buffers = {};
		std::unordered_map< GLuint, alloc_t > index_buffers = {};
		std::unordered_map< GLuint, alloc_t > textures = {};
		std::unordered_map< GLuint, framebuffer_t > framebuffers = {};
		std::unordered_map< GLuint, GLuint > buffers_framebuffers = {};
		std::unordered_map< GLuint, GLuint > textures_framebuffers = {};
		std::unordered_map< GLuint, program_t > programs = {};
	} m_opengl = {};

};

extern MemoryWatcher* g_memory_watcher;

}
