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
	MemoryWatcher();
	~MemoryWatcher();

	// memory stuff
	void New( const Base* object, const size_t size, const std::string& file, const size_t line );
	void Delete( const Base* object, const std::string& file, const size_t line );
	void* Malloc( const size_t size, const std::string& file, const size_t line );
	void* Realloc( void* ptr, const size_t size, const std::string& file, const size_t line );
	unsigned char *Ptr( unsigned char* ptr, const size_t offset, const size_t size, const std::string& file, const size_t line  );
	void Free( void* ptr, const std::string& file, const size_t line );

	
	// opengl stuff
	void GLGenBuffers( GLsizei n, GLuint * buffers, const std::string& file, const size_t line );
	void GLBindBuffer( GLenum target, GLuint buffer, const std::string& file, const size_t line );
	void GLBufferData( GLenum target, GLsizeiptr size, const void * data, GLenum usage, const std::string& file, const size_t line );
	void GLDeleteBuffers( GLsizei n, const GLuint * buffers, const std::string& file, const size_t line );
	void GLGenTextures( GLsizei n, GLuint * textures, const std::string& file, const size_t line );
	void GLBindTexture( GLenum target, GLuint texture, const std::string& file, const size_t line );
	void GLTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels, const std::string& file, const size_t line );
	void GLTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, const std::string& file, const size_t line );
	void GLDeleteTextures( GLsizei n, GLuint * textures, const std::string& file, const size_t line );
	void GLDrawElements( GLenum mode, GLsizei count, GLenum type, const void * indices, const std::string& file, const size_t line );
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
	std::mutex m_mutex;
	
	void Log( const std::string& text );
	
	struct object_info_t {
		const void* ptr;
		size_t size;
		std::string object_name;
		std::string object_namespace;
		std::string source;
	};
	std::unordered_map<const void*, object_info_t> m_allocated_objects;
	
	struct alloc_t {
		size_t size;
		std::string source;
	};
	std::unordered_map<const void*, alloc_t> m_allocated_memory;

	struct {
		GLuint current_vertex_buffer = 0;
		GLuint current_index_buffer = 0;
		GLuint current_texture = 0;
		std::unordered_set<GLuint> buffers;
		std::unordered_map<GLuint, alloc_t> vertex_buffers;
		std::unordered_map<GLuint, alloc_t> index_buffers;
		std::unordered_map<GLuint, alloc_t> textures;
	} m_opengl;
	
};

extern MemoryWatcher* g_memory_watcher;

}
