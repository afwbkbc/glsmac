#include "MemoryWatcher.h"

#include <unordered_map>
#include <iostream>
#include <algorithm>

// to avoid being intercepted by macros in Base.h again
void glGenBuffers_real( GLsizei n, GLuint * buffers ) {
	glGenBuffers( n, buffers );
}
void glBindBuffer_real( GLenum target, GLuint buffer ) {
	glBindBuffer( target, buffer );
}
void glBufferData_real( GLenum target, GLsizeiptr size, const void * data, GLenum usage ) {
	glBufferData( target, size, data, usage );
}
void glDeleteBuffers_real( GLsizei n, const GLuint * buffers ) {
	glDeleteBuffers( n, buffers );
}

#include "base/Base.h"

using namespace std;
using namespace base;

namespace debug {

MemoryWatcher g_memory_watcher;

void MemoryWatcher::New( const Base* object, const size_t size, const string& file, const size_t line ) {
	lock_guard<mutex> guard( m_mutex );
	
	const string source = file + ":" + to_string(line);
	if ( m_allocated_objects.find( object ) != m_allocated_objects.end() ) {
		throw runtime_error( "double-allocation detected @" + source );
	}
	m_allocated_objects[ object ] = {
		object,
		size,
		object->GetName(),
		object->GetClassName(),
		source,
	};
	
	DEBUG_STAT_INC( objects_created );
	DEBUG_STAT_INC( objects_active );
	DEBUG_STAT_CHANGE_BY( bytes_allocated, size );

	// VERY spammy
	//Log( "allocated " + to_string( size ) + "b for " + object->GetNamespace() + " @" + source );
}

void MemoryWatcher::Delete( const Base* object, const string& file, const size_t line ) {
	lock_guard<mutex> guard( m_mutex );
	
	const string source = file + ":" + to_string(line);
	auto it = m_allocated_objects.find( object );
	if ( it == m_allocated_objects.end() ) {
		throw runtime_error( "delete on non-allocated object detected @" + source );
	}
	auto& obj = it->second;
	
	DEBUG_STAT_INC( objects_destroyed );
	DEBUG_STAT_DEC( objects_active );
	DEBUG_STAT_CHANGE_BY( bytes_allocated, -obj.size );

	// VERY spammy
	//Log( "freed " + to_string( obj.size ) + "b from " + object->GetNamespace() + " @" + source );
	
	m_allocated_objects.erase( it );
}

void MemoryWatcher::GLGenBuffers( GLsizei n, GLuint * buffers, const string& file, const size_t line ) {
	lock_guard<mutex> guard( m_mutex );
	const string source = file + ":" + to_string(line);
	
	if ( n != 1 ) {
		throw runtime_error( "glGenBuffers with size " + to_string(n) + ", suspicious, is it a typo? @" + source );
	}
	glGenBuffers_real( n, buffers );
	Log( "created opengl buffer " + to_string( *buffers ) + " @" + source );
	
	if ( m_opengl.buffers.find( *buffers ) != m_opengl.buffers.end() ) {
		throw runtime_error( "glGenBuffers buffer id overlap @" + source );
	}
	m_opengl.buffers.insert( *buffers );
	
	DEBUG_STAT_INC( opengl_buffers_count );
}

void MemoryWatcher::GLBindBuffer( GLenum target, GLuint buffer, const string& file, const size_t line ) {
	lock_guard<mutex> guard( m_mutex );
	const string source = file + ":" + to_string(line);
	
	if ( target != GL_ARRAY_BUFFER && target != GL_ELEMENT_ARRAY_BUFFER ) {
		throw runtime_error( "glBindBuffer unknown target " + to_string(target) + " @" + source );
	}

	if ( target == GL_ARRAY_BUFFER ) {
		if ( m_opengl.current_vertex_buffer == buffer ) {
			throw runtime_error( "glBindBuffer called with same vertex buffer ( " + to_string( buffer ) + " ) as already bound @" + source );
		}
		if ( buffer != 0 ) {
			if ( m_opengl.current_vertex_buffer != 0 ) {
				throw runtime_error( "glBindBuffer called on already bound vertex buffer ( " + to_string( m_opengl.current_vertex_buffer ) + ", " + to_string( buffer ) + " ) @" + source );
			}
			if ( m_opengl.index_buffers.find( buffer ) != m_opengl.index_buffers.end() ) {
				throw runtime_error( "glBindBuffer buffer " + to_string( buffer ) + " was previously used as index buffer, but now vertex @" + source );
			}
			m_opengl.vertex_buffers.insert( buffer );
			//Log("binding to opengl vertex buffer " + to_string( buffer ) + " @" + source );
			if ( m_opengl.vertex_buffer_sizes.find( buffer ) == m_opengl.vertex_buffer_sizes.end() ) {
				m_opengl.vertex_buffer_sizes[ buffer ] = 0;
			}
		}
		else {
			if ( m_opengl.current_vertex_buffer != 0 ) {
				//Log("unbinding from opengl vertex buffer " + to_string( m_opengl.current_vertex_buffer ) + " @" + source );
			}
		}
		m_opengl.current_vertex_buffer = buffer;
	}
	else {
		if ( m_opengl.current_index_buffer == buffer ) {
			throw runtime_error( "glBindBuffer called with same index buffer ( " + to_string( buffer ) + " ) as already bound@" + source );
		}
		if ( buffer != 0 ) {
			if ( m_opengl.current_index_buffer != 0 ) {
				throw runtime_error( "glBindBuffer called on already bound index buffer ( " + to_string( m_opengl.current_vertex_buffer ) + ", " + to_string( buffer ) + " )@" + source );
			}
			if ( m_opengl.vertex_buffers.find( buffer ) != m_opengl.vertex_buffers.end() ) {
				throw runtime_error( "glBindBuffer buffer " + to_string( buffer ) + " was previously used as vertex buffer, but now index @" + source );
			}
			m_opengl.index_buffers.insert( buffer );
			//Log("binding to opengl index buffer " + to_string( buffer ) + " @" + source );
			if ( m_opengl.index_buffer_sizes.find( buffer ) == m_opengl.index_buffer_sizes.end() ) {
				m_opengl.index_buffer_sizes[ buffer ] = 0;
			}
		}
		else {
			if ( m_opengl.current_index_buffer != 0 ) {
				//Log("unbinding from opengl index buffer " + to_string( m_opengl.current_index_buffer  ) + " @" + source );
			}
		}
		m_opengl.current_index_buffer = buffer;
	}
	
	glBindBuffer_real( target, buffer );
}

void MemoryWatcher::GLBufferData( GLenum target, GLsizeiptr size, const void * data, GLenum usage, const string& file, const size_t line ) {
	if ( target != GL_ARRAY_BUFFER && target != GL_ELEMENT_ARRAY_BUFFER ) {
		throw runtime_error( "glBufferData unknown target " + to_string(target) );
	}
	
	lock_guard<mutex> guard( m_mutex );
	const string source = file + ":" + to_string(line);
	
	if ( target == GL_ARRAY_BUFFER ) {
		if ( m_opengl.current_vertex_buffer == 0 ) {
			throw runtime_error( "glBufferData called without bound vertex buffer @" + source );
		}
		size_t old_size = m_opengl.vertex_buffer_sizes.at( m_opengl.current_vertex_buffer );
		if ( old_size > 0 ) {
			//Log( "freeing " + to_string( size ) + " bytes from opengl vertex buffer " + to_string( m_opengl.current_vertex_buffer ) + " @" + source );
			DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, -old_size );
		}
		//Log( "loading " + to_string( size ) + " bytes into opengl vertex buffer " + to_string( m_opengl.current_vertex_buffer ) + " @" + source );
		m_opengl.vertex_buffer_sizes[ m_opengl.current_vertex_buffer ] = size;
		DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, size );
	}
	else {
		if ( m_opengl.current_index_buffer == 0 ) {
			throw runtime_error( "glBufferData called without bound index buffer @" + source );
		}
		size_t old_size = m_opengl.index_buffer_sizes.at( m_opengl.current_index_buffer );
		if ( old_size > 0 ) {
			//Log( "freeing " + to_string( size ) + " bytes from opengl index buffer " + to_string( m_opengl.current_index_buffer ) + " @" + source );
			DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, -old_size );
		}
		//Log( "loading " + to_string( size ) + " bytes into opengl index buffer " + to_string( m_opengl.current_index_buffer ) + " @" + source );
		m_opengl.index_buffer_sizes[ m_opengl.current_index_buffer ] = size;
		DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, size );
	}
	
	glBufferData_real( target, size, data, usage );
}

void MemoryWatcher::GLDeleteBuffers( GLsizei n, const GLuint * buffers, const string& file, const size_t line ) {
	lock_guard<mutex> guard( m_mutex );
	const string source = file + ":" + to_string(line);
	
	if ( n != 1 ) {
		throw runtime_error( "glDeleteBuffers with size " + to_string(n) + ", suspicious, is it a typo? @" + source );
	}
	
	if ( m_opengl.buffers.find( *buffers ) == m_opengl.buffers.end() ) {
		throw runtime_error( "glDeleteBuffers buffer does not exist @" + source );
	}
	
	Log( "destroying opengl buffer " + to_string( *buffers ) + " @" + source );
	
	m_opengl.buffers.erase( *buffers );
	DEBUG_STAT_DEC( opengl_buffers_count );
	
	glDeleteBuffers_real( n, buffers );
}

struct sort_method
{
    inline bool operator() (const MemoryWatcher::statistics_item_t& struct1, const MemoryWatcher::statistics_item_t& struct2)
    {
        return (struct1.size > struct2.size);
    }
};

const MemoryWatcher::statistics_result_t MemoryWatcher::GetLargestMemoryConsumerClasses( size_t count ) {
	lock_guard<mutex> guard( m_mutex );
	
	statistics_t stats;
	statistics_t::iterator it_dst;
	for ( auto& it_src : m_allocated_objects ) {
		const string& key = it_src.second.object_namespace;
		it_dst = stats.find( key );
		if ( it_dst == stats.end() ) {
			stats[ key ] = { 0, 0, key };
		}
		stats[ key ].size += it_src.second.size;
		stats[ key ].count++;
	}
	
	statistics_result_t result = {};
	
	size_t i = 0;
	for ( auto& it : stats ) {
		result.push_back( it.second );
		if ( ++i == count ) {
			break;
		}
	}
	
	sort( result.begin(), result.end(), sort_method() );
	
	if ( result.size() > count ) {
		result.resize( count );
	}
	
	return result;
}

void MemoryWatcher::Log( const string& text ) {
	cout << "<MemoryWatcher> " << text << endl;
}

}
