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

using namespace std;
using namespace base;

namespace debug {

class MemoryWatcher {
public:
	MemoryWatcher();
	~MemoryWatcher();
	
	void New( const Base* object, const size_t size, const string& file, const size_t line );
	void Delete( const Base* object, const string& file, const size_t line );
	void GLGenBuffers( GLsizei n, GLuint * buffers, const string& file, const size_t line );
	void GLBindBuffer( GLenum target, GLuint buffer, const string& file, const size_t line );
	void GLBufferData( GLenum target, GLsizeiptr size, const void * data, GLenum usage, const string& file, const size_t line );
	void GLDeleteBuffers( GLsizei n, const GLuint * buffers, const string& file, const size_t line );
	
	typedef struct {
		const void* ptr;
		size_t size;
		string object_name;
		string object_namespace;
		string source;
	} object_info_t;
	
	typedef struct {
		size_t size;
		size_t count;
		string key;
	} statistics_item_t;
	typedef unordered_map< string, statistics_item_t > statistics_t;
	typedef vector< statistics_item_t > statistics_result_t;
	
	const statistics_result_t GetLargestMemoryConsumerClasses( size_t count = 10 );
	
private:
	mutex m_mutex;
	
	void Log( const string& text );
	
	unordered_map<const void*, object_info_t> m_allocated_objects;
	
	struct {
		GLuint current_vertex_buffer = 0;
		GLuint current_index_buffer = 0;
		unordered_set<GLuint> buffers;
		unordered_set<GLuint> vertex_buffers;
		unordered_set<GLuint> index_buffers;
		unordered_map<GLuint, size_t> vertex_buffer_sizes;
		unordered_map<GLuint, size_t> index_buffer_sizes;
	} m_opengl;
	
};

extern MemoryWatcher* g_memory_watcher;

}
