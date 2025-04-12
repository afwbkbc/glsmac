#ifdef DEBUG

#include <unordered_map>
#include <algorithm>
#include <string>
#include <sstream>

#include "MemoryWatcher.h"

#include "util/LogHelper.h"

// to avoid being intercepted by macros in Base.h again

void* malloc_real( const size_t size ) {
	return malloc( size );
}

void* realloc_real( void* ptr, const size_t size ) {
	return realloc( ptr, size );
}

void free_real( void* ptr ) {
	free( ptr );
}

void glGenBuffers_real( GLsizei n, GLuint* buffers ) {
	glGenBuffers( n, buffers );
}

void glBindBuffer_real( GLenum target, GLuint buffer ) {
	glBindBuffer( target, buffer );
}

void glBufferData_real( GLenum target, GLsizeiptr size, const void* data, GLenum usage ) {
	glBufferData( target, size, data, usage );
}

void glDeleteBuffers_real( GLsizei n, const GLuint* buffers ) {
	glDeleteBuffers( n, buffers );
}

void glGenTextures_real( GLsizei n, GLuint* textures ) {
	glGenTextures( n, textures );
}

void glBindTexture_real( GLenum target, GLuint texture ) {
	glBindTexture( target, texture );
}

void glTexImage2D_real( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels ) {
	glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
}

void glTexSubImage2D_real( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels ) {
	glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
}

void glDeleteTextures_real( GLsizei n, GLuint* textures ) {
	glDeleteTextures( n, textures );
}

void glGenFramebuffers_real( GLsizei n, GLuint* buffers ) {
	glGenFramebuffers( n, buffers );
}

void glBindFramebuffer_real( GLenum target, GLuint buffer ) {
	glBindFramebuffer( target, buffer );
}

void glFramebufferTexture2D_real( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level ) {
	glFramebufferTexture2D( target, attachment, textarget, texture, level );
}

void glDeleteFramebuffers_real( GLsizei n, const GLuint* buffers ) {
	glDeleteFramebuffers( n, buffers );
}

GLuint glCreateProgram_real() {
	return glCreateProgram();
}

void glLinkProgram_real( GLuint program ) {
	glLinkProgram( program );
}

void glValidateProgram_real( GLuint program ) {
	glValidateProgram( program );
}

void glUseProgram_real( GLuint program ) {
	glUseProgram( program );
}

void glDeleteProgram_real( GLuint program ) {
	glDeleteProgram( program );
}

void glDrawElements_real( GLenum mode, GLsizei count, GLenum type, const void* indices ) {
	glDrawElements( mode, count, type, indices );
}

void glDrawElementsInstanced_real( GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount ) {
	glDrawElementsInstanced( mode, count, type, indices, primcount );
}

void glDrawArrays_real( GLenum mode, GLint first, GLsizei count ) {
	glDrawArrays( mode, first, count );
}

#include "common/Common.h"

namespace debug {

MemoryWatcher* g_memory_watcher = nullptr;

MemoryWatcher::MemoryWatcher( const bool memory_debug, const bool is_quiet )
	: m_memory_debug( memory_debug )
	, m_is_quiet( is_quiet ) {
	ASSERT( !g_memory_watcher, "duplicate MemoryWatcher instantiation" );
	g_memory_watcher = this;
}

MemoryWatcher::~MemoryWatcher() {

	Log( "Checking for possible memory/opengl leaks..." );

	bool any_leaks = false;
#define CHECK_LEAKS( _where ) \
    if ( !_where.empty() ) { \
        Log( "WARNING: " + std::to_string( _where.size() ) + " objects were never freed (possible memory leaks?):", true ); \
        for (auto& o : _where) { \
            std::stringstream ptrstr; \
            ptrstr << o.first; \
            Log( "    (" + ptrstr.str() + ") @" + o.second.source, true ); \
        } \
        any_leaks = true; \
    }
	if ( m_memory_debug ) {
		CHECK_LEAKS( m_allocated_objects )
		CHECK_LEAKS( m_allocated_memory )
	}
	CHECK_LEAKS( m_opengl.vertex_buffers )
	CHECK_LEAKS( m_opengl.index_buffers )
	CHECK_LEAKS( m_opengl.textures )

#undef CHECK_LEAKS

	if ( !any_leaks ) {
		Log( "No memory/opengl leaks detected." );
		if ( !m_memory_debug ) {
			Log( "Memory allocations were not checked. Run with --memorydebug to test them too." );
		}
	}
}

void MemoryWatcher::New( const void* object, const size_t size, const std::string& file, const size_t line ) {
	if ( !m_memory_debug ) {
		return;
	}

	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( m_allocated_objects.find( object ) == m_allocated_objects.end(), "new double-allocation detected @" + source );
	m_allocated_objects[ object ] = {
		object,
		size,
		"const void*", //object->GetName(), // do we need?
		"const void*", //object->GetClassName(), // do we need?
		source,
	};

	DEBUG_STAT_INC( objects_created );
	DEBUG_STAT_INC( objects_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, size );

	// VERY spammy
	//Log( "Allocated " + std::to_string( size ) + "b for " + object->GetNamespace() + " @" + source );
}

void MemoryWatcher::MaybeDelete( const void* object ) {
	if ( !m_memory_debug ) {
		return;
	}

	std::lock_guard guard( m_mutex );

	auto it = m_allocated_objects.find( object );
	if ( it != m_allocated_objects.end() ) {
		auto& obj = it->second;
		DEBUG_STAT_INC( objects_destroyed );
		DEBUG_STAT_DEC( objects_active );
		DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );
		m_allocated_objects.erase( it );
	}
}

void MemoryWatcher::Delete( const void* object, const std::string& file, const size_t line ) {
	if ( !m_memory_debug ) {
		return;
	}

	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	auto it = m_allocated_objects.find( object );
	ASSERT( it != m_allocated_objects.end(), "delete on non-allocated object detected @" + source );

	auto& obj = it->second;

	DEBUG_STAT_INC( objects_destroyed );
	DEBUG_STAT_DEC( objects_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );

	// VERY spammy
	//Log( "Freed " + std::to_string( obj.size ) + "b from " + object->GetNamespace() + " @" + source );

	m_allocated_objects.erase( it );
}

void* MemoryWatcher::Malloc( const size_t size, const std::string& file, const size_t line ) {
	if ( !m_memory_debug ) {
		return malloc_real( size );
	}

	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( size > 0, "allocation of size 0 @" + source );

	void* ptr = malloc_real( size );
	ASSERT( m_allocated_memory.find( ptr ) == m_allocated_memory.end(), "malloc double-allocation detected @" + source );

	m_allocated_memory[ ptr ] = {
		size,
		source
	};

	DEBUG_STAT_INC( buffers_created );
	DEBUG_STAT_INC( buffers_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, size );

	// VERY spammy
	//Log( "Allocated " + std::to_string( size ) + "b for " + std::to_string( (long int)ptr ) + " @" + source );

	return ptr;
}

void* MemoryWatcher::Realloc( void* ptr, const size_t size, const std::string& file, const size_t line ) {

	if ( !m_memory_debug ) {
		return realloc_real( ptr, size );
	}

	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( ptr, "reallocation of null @" + source );

	ASSERT( size > 0, "reallocation of size 0 @" + source );

	auto it = m_allocated_memory.find( ptr );
	ASSERT( it != m_allocated_memory.end(), "realloc on non-allocated object detected @" + source );

	auto& obj = it->second;

	DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );

	// VERY spammy
	//Log( "Freed " + std::to_string( obj.size ) + "b from " + std::to_string( (long int)ptr ) + " @" + source );

	m_allocated_memory.erase( it );

	ptr = realloc_real( ptr, size );

	ASSERT( m_allocated_memory.find( ptr ) == m_allocated_memory.end(), "realloc double-allocation detected @" + source );

	m_allocated_memory[ ptr ] = {
		size,
		source
	};

	DEBUG_STAT_CHANGE_BY( heap_allocated_size, size );

	// VERY spammy
	//Log( "Allocated " + std::to_string( size ) + "b for " + std::to_string( (long int)ptr ) + " @" + source );

	return ptr;
}

unsigned char* MemoryWatcher::Ptr( unsigned char* ptr, const size_t offset, const size_t size, const std::string& file, const size_t line ) {
	if ( m_memory_debug ) {
		std::lock_guard guard( m_mutex );
		const std::string source = file + ":" + std::to_string( line );

		ASSERT( ptr, "ptr is null @" + source );

		auto it = m_allocated_memory.find( ptr );
		ASSERT( it != m_allocated_memory.end(), "ptr on non-allocated pointer @" + source );

		ASSERT( offset + size <= it->second.size,
			"ptr overflow (" + std::to_string( offset ) + " + " + std::to_string( size ) + " > " + std::to_string( it->second.size ) + ") @" + source + " (allocated @" + it->second.source + ")"
		);
	}
	return ptr + offset;
}

void MemoryWatcher::Free( void* ptr, const std::string& file, const size_t line ) {
	if ( !m_memory_debug ) {
		free_real( ptr );
		return;
	}

	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	auto it = m_allocated_memory.find( ptr );
	ASSERT( it != m_allocated_memory.end(), "free on non-allocated object " + std::to_string( (long long)ptr ) + " detected @" + source );

	auto& obj = it->second;

	free_real( ptr );

	DEBUG_STAT_INC( buffers_destroyed );
	DEBUG_STAT_DEC( buffers_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );

	// VERY spammy
	//Log( "Freed " + std::to_string( obj.size ) + "b from " + std::to_string( (long int)ptr ) + " @" + source );

	m_allocated_memory.erase( it );
}

void MemoryWatcher::GLGenBuffers( GLsizei n, GLuint* buffers, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( n == 1, "glGenBuffers with size " + std::to_string( n ) + ", suspicious, is it a typo? @" + source );

	glGenBuffers_real( n, buffers );
	//Log( "Created opengl buffer " + std::to_string( *buffers ) + " @" + source );

	ASSERT( m_opengl.buffers.find( *buffers ) == m_opengl.buffers.end(), "glGenBuffers buffer id overlap @" + source );

	m_opengl.buffers.insert( *buffers );

	if ( m_opengl.current_framebuffer ) {
		ASSERT( m_opengl.current_framebuffer_mode == FM_BIND, "glGenBuffers buffer generated within framebuffer scope but framebuffer mode is wrong @" + source );
		auto it = m_opengl.framebuffers.find( m_opengl.current_framebuffer );
		ASSERT( it != m_opengl.framebuffers.end(), "glGenBuffers framebuffer not found @" + source );
		ASSERT( it->second.generated_buffers.find( *buffers ) == it->second.generated_buffers.end(), "glGenBuffers framebuffer buffers overlap @" + source );
		//Log( "glGenBuffers linking buffer " + std::to_string( *buffers ) + " to framebuffer " + std::to_string( m_opengl.current_framebuffer ) + " @" + source );
		it->second.generated_buffers.insert( *buffers );
		ASSERT( m_opengl.buffers_framebuffers.find( *buffers ) == m_opengl.buffers_framebuffers.end(), "glGenBuffers buffers_framebuffers overlap @" + source );
		m_opengl.buffers_framebuffers[ *buffers ] = m_opengl.current_framebuffer;
	}

	DEBUG_STAT_INC( opengl_buffers_count );
}

void MemoryWatcher::GLBindBuffer( GLenum target, GLuint buffer, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER,
		"glBindBuffer unknown target " + std::to_string( target ) + " @" + source
	);

	if ( buffer ) {
		ASSERT( m_opengl.buffers.find( buffer ) != m_opengl.buffers.end(), "glBindBuffer buffer " + std::to_string( buffer ) + " was not generated @" + source );
	}

	ASSERT( !m_opengl.current_program, "glBindBuffer but some program already bound @" + source );

	auto it = m_opengl.buffers_framebuffers.find( buffer );
	if ( it != m_opengl.buffers_framebuffers.end() ) {
		ASSERT( it->second == m_opengl.current_framebuffer, "glBindBuffer owner framebuffer not bound @" + source );
		ASSERT( m_opengl.current_framebuffer_mode == FM_BIND || m_opengl.current_framebuffer_mode == FM_DRAW,
			"glBindBuffer owner framebuffer in wrong mode @" + source
		);
	}

	if ( target == GL_ARRAY_BUFFER ) {
		ASSERT( m_opengl.current_vertex_buffer != buffer,
			"glBindBuffer called with same vertex buffer ( " + std::to_string( buffer ) + " ) as already bound @" + source
		);
		if ( buffer != 0 ) {
			ASSERT( m_opengl.current_vertex_buffer == 0,
				"glBindBuffer called on already bound vertex buffer ( " + std::to_string( m_opengl.current_vertex_buffer ) + ", " + std::to_string( buffer ) + " ) @" + source
			);
			ASSERT( m_opengl.index_buffers.find( buffer ) == m_opengl.index_buffers.end(),
				"glBindBuffer buffer " + std::to_string( buffer ) + " was previously used as index buffer, but now vertex @" + source
			);
			if ( m_opengl.vertex_buffers.find( buffer ) == m_opengl.vertex_buffers.end() ) {
				m_opengl.vertex_buffers[ buffer ] = {
					0,
					source
				};
			}
			//Log("Binding to opengl vertex buffer " + std::to_string( buffer ) + " @" + source );
		}
		else {
			if ( m_opengl.current_vertex_buffer != 0 ) {
				//Log("Unbinding from opengl vertex buffer " + std::to_string( m_opengl.current_vertex_buffer ) + " @" + source );
			}
		}
		m_opengl.current_vertex_buffer = buffer;
	}
	else {
		ASSERT( m_opengl.current_index_buffer != buffer,
			"glBindBuffer called with same index buffer ( " + std::to_string( buffer ) + " ) as already bound@" + source
		);
		if ( buffer != 0 ) {
			ASSERT( m_opengl.current_index_buffer == 0,
				"glBindBuffer called on already bound index buffer ( " + std::to_string( m_opengl.current_vertex_buffer ) + ", " + std::to_string( buffer ) + " )@" + source
			);
			ASSERT( m_opengl.vertex_buffers.find( buffer ) == m_opengl.vertex_buffers.end(),
				"glBindBuffer buffer " + std::to_string( buffer ) + " was previously used as vertex buffer, but now index @" + source
			);
			if ( m_opengl.index_buffers.find( buffer ) == m_opengl.index_buffers.end() ) {
				m_opengl.index_buffers[ buffer ] = {
					0,
					source
				};
			}
			//Log("Binding to opengl index buffer " + std::to_string( buffer ) + " @" + source );
		}
		else {
			if ( m_opengl.current_index_buffer != 0 ) {
				//Log("Unbinding from opengl index buffer " + std::to_string( m_opengl.current_index_buffer  ) + " @" + source );
			}
		}
		m_opengl.current_index_buffer = buffer;
	}

	glBindBuffer_real( target, buffer );
}

void MemoryWatcher::GLBufferData( GLenum target, GLsizeiptr size, const void* data, GLenum usage, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT(
		target == GL_ARRAY_BUFFER ||
			target == GL_ELEMENT_ARRAY_BUFFER,
		"glBufferData unknown target " + std::to_string( target )
	);

	if ( target == GL_ARRAY_BUFFER ) {
		ASSERT( m_opengl.current_vertex_buffer != 0, "glBufferData called without bound vertex buffer @" + source );

		auto it = m_opengl.vertex_buffers.find( m_opengl.current_vertex_buffer );
		ASSERT( it != m_opengl.vertex_buffers.end(), "opengl vertex buffer not bound" );
		if ( it->second.size > 0 ) {
			//Log( "Freeing " + std::to_string( size ) + " bytes from opengl vertex buffer " + std::to_string( m_opengl.current_vertex_buffer ) + " @" + source );
			DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, -it->second.size );
		}
		//Log( "Loading " + std::to_string( size ) + " bytes into opengl vertex buffer " + std::to_string( m_opengl.current_vertex_buffer ) + " @" + source );
		it->second.size = (size_t)size;
		DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, size );
		DEBUG_STAT_INC( opengl_vertex_buffers_updates );
	}
	else {
		ASSERT( m_opengl.current_index_buffer != 0, "glBufferData called without bound index buffer @" + source );

		auto it = m_opengl.index_buffers.find( m_opengl.current_index_buffer );
		ASSERT( it != m_opengl.index_buffers.end(), "opengl index buffer not bound" );
		if ( it->second.size > 0 ) {
			//Log( "Freeing " + std::to_string( size ) + " bytes from opengl index buffer " + std::to_string( m_opengl.current_index_buffer ) + " @" + source );
			DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, -it->second.size );
		}
		//Log( "Loading " + std::to_string( size ) + " bytes into opengl index buffer " + std::to_string( m_opengl.current_index_buffer ) + " @" + source );
		it->second.size = (size_t)size;
		DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, size );
		DEBUG_STAT_INC( opengl_index_buffers_updates );
	}

	glBufferData_real( target, size, data, usage );
}

void MemoryWatcher::GLDeleteBuffers( GLsizei n, const GLuint* buffers, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( n == 1, "glDeleteBuffers with size " + std::to_string( n ) + ", suspicious, is it a typo? @" + source );

	ASSERT( m_opengl.buffers.find( *buffers ) != m_opengl.buffers.end(), "glDeleteBuffers buffer does not exist @" + source );

	auto it_vertex = m_opengl.vertex_buffers.find( *buffers );
	auto it_index = m_opengl.index_buffers.find( *buffers );

	ASSERT( it_vertex == m_opengl.vertex_buffers.end() || it_index == m_opengl.index_buffers.end(),
		"glDeleteBuffers buffer is both vertex buffer and index buffer, is it a bug? @" + source
	);
	if ( it_vertex != m_opengl.vertex_buffers.end() ) {
		ASSERT( m_opengl.current_vertex_buffer != *buffers, "glDeleteBuffers destroying vertex buffer while it's still bound @" + source );
		//Log( "Destroying opengl vertex buffer " + std::to_string( *buffers ) + " @" + source );
		DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, -it_vertex->second.size );
		m_opengl.vertex_buffers.erase( it_vertex );
	}
	if ( it_index != m_opengl.index_buffers.end() ) {
		ASSERT( m_opengl.current_index_buffer != *buffers, "glDeleteBuffers destroying index buffer while it's still bound @" + source );
		//Log( "Destroying opengl index buffer " + std::to_string( *buffers ) + " @" + source );
		DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, -it_index->second.size );
		m_opengl.index_buffers.erase( it_index );
	}

	if ( m_opengl.current_framebuffer ) {
		auto it = m_opengl.framebuffers.find( m_opengl.current_framebuffer );
		ASSERT( it != m_opengl.framebuffers.end(), "glDeleteBuffers framebuffer not found @" + source );
		auto it2 = it->second.generated_buffers.find( *buffers );
		ASSERT( it2 != it->second.generated_buffers.end(), "glDeleteBuffers buffer deleted within framebuffer scope but not generated within this framebuffer scope @" + source );
		//Log( "glDeleteBuffers unlinking buffer " + std::to_string( *buffers ) + " from framebuffer " + std::to_string( m_opengl.current_framebuffer ) + " @" + source );
		it->second.generated_buffers.erase( it2 );
	}

	auto it = m_opengl.buffers_framebuffers.find( *buffers );
	if ( it != m_opengl.buffers_framebuffers.end() ) {
		ASSERT( it->second == m_opengl.current_framebuffer, "glDeleteBuffers deleting buffer while owner framebuffer not bound @" + source );
		ASSERT( m_opengl.current_framebuffer_mode == FM_BIND, "glDeleteBuffers deleting buffer but owner framebuffer in wrong mode @" + source );
		m_opengl.buffers_framebuffers.erase( it );
	}

	m_opengl.buffers.erase( *buffers );
	DEBUG_STAT_DEC( opengl_buffers_count );

	glDeleteBuffers_real( n, buffers );
}

void MemoryWatcher::GLGenTextures( GLsizei n, GLuint* textures, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( n == 1, "glGenTextures with size " + std::to_string( n ) + ", suspicious, is it a typo? @" + source );

	glGenTextures_real( n, textures );
	//Log( "Created opengl texture " + std::to_string( *textures ) + " @" + source );

	ASSERT( m_opengl.textures.find( *textures ) == m_opengl.textures.end(), "glGenTextures texture id overlap @" + source );

	m_opengl.textures[ *textures ] = {
		0,
		source
	};

	if ( m_opengl.current_framebuffer ) {
		ASSERT( m_opengl.current_framebuffer_mode == FM_BIND, "glGenTextures texture generated within framebuffer scope but framebuffer mode is wrong @" + source );
		auto it = m_opengl.framebuffers.find( m_opengl.current_framebuffer );
		ASSERT( it != m_opengl.framebuffers.end(), "glGenTextures framebuffer not found @" + source );
		ASSERT( it->second.generated_textures.find( *textures ) == it->second.generated_textures.end(), "glGenTextures framebuffer textures overlap @" + source );
		it->second.generated_textures.insert( *textures );
		ASSERT( m_opengl.textures_framebuffers.find( *textures ) == m_opengl.textures_framebuffers.end(), "glGenTextures textures_framebuffers overlap @" + source );
		m_opengl.textures_framebuffers[ *textures ] = m_opengl.current_framebuffer;
	}

	DEBUG_STAT_INC( opengl_textures_count );
}

void MemoryWatcher::GLBindTexture( GLenum target, GLuint texture, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( target == GL_TEXTURE_2D, "glBindTexture unknown target " + std::to_string( target ) + " @" + source );

	if ( texture ) {
		ASSERT( m_opengl.textures.find( texture ) != m_opengl.textures.end(), "glBindTexture texture " + std::to_string( texture ) + " was not generated @" + source );
	}

	ASSERT( m_opengl.current_texture != texture, "glBindTexture called with same texture ( " + std::to_string( texture ) + " ) as already bound @" + source );

	auto it = m_opengl.textures_framebuffers.find( texture );
	if ( it != m_opengl.textures_framebuffers.end() ) {
		if ( it->second == m_opengl.current_framebuffer ) {
			ASSERT( m_opengl.current_framebuffer_mode == FM_BIND,
				"glBindTexture owner framebuffer in wrong mode @" + source
			);
		}
		auto it2 = m_opengl.framebuffers.find( it->second );
		ASSERT( it2 != m_opengl.framebuffers.end(), "glBindTexture framebuffer not found @" + source );
		if ( it2->second.textures.render == texture || it2->second.textures.depth == texture ) {
			// TODO: fix ASSERT( it->second != m_opengl.current_framebuffer, "glBindTexture tried to bind but associated framebuffer is bound too @" + source );
		}
	}

	if ( texture != 0 ) {
		ASSERT( m_opengl.current_texture == 0, "glBindTexture called on already bound texture ( " + std::to_string( m_opengl.current_texture ) + ", " + std::to_string( texture ) + " ) @" + source );
		//Log("Binding to opengl texture " + std::to_string( texture ) + " @" + source );
		if ( m_opengl.textures.find( texture ) == m_opengl.textures.end() ) {
			m_opengl.textures[ texture ] = {
				0,
				source
			};
		}
	}
	else {
		if ( m_opengl.current_texture != 0 ) {
			//Log("Unbinding from opengl texture " + std::to_string( m_opengl.current_vertex_buffer ) + " @" + source );
		}
	}
	m_opengl.current_texture = texture;

	glBindTexture_real( target, texture );
}

void MemoryWatcher::GLTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( target == GL_TEXTURE_2D, "glTexImage2D unknown target " + std::to_string( target ) + " @" + source );
	ASSERT( level == 0, "glTexImage2D unknown level " + std::to_string( level ) + " @" + source );
	ASSERT( width >= 0, "glTexImage2D zero width" );
	ASSERT( height >= 0, "glTexImage2D zero height" );
	ASSERT( border == 0, "glTexImage2D unknown border " + std::to_string( border ) + " @" + source );
	ASSERT(
		type == GL_UNSIGNED_BYTE ||
			type == GL_UNSIGNED_INT ||
			type == GL_FLOAT,
		"glTexImage2D unknown type " + std::to_string( type ) + " @" + source
	);
	ASSERT( m_opengl.current_texture != 0, "glTexImage2D called without bound texture @" + source );

	size_t internalbpp, bpp;
	switch ( internalformat ) {
		case GL_RED: {
			internalbpp = 1;
			break;
		}
		case GL_RGBA8: {
			internalbpp = 4;
			break;
		}
		case GL_R32UI: {
			internalbpp = 1;
			break;
		}
		case GL_DEPTH_COMPONENT: {
			internalbpp = 1;
			break;
		}
		default:
			THROW( "glTexImage2D unknown internal format " + std::to_string( internalformat ) + " @" + source );
	}
	switch ( format ) {
		case GL_RED: {
			bpp = 1;
			break;
		}
		case GL_RGBA: {
			bpp = 4;
			break;
		}
		case GL_RED_INTEGER: {
			bpp = 1;
			break;
		}
		case GL_DEPTH_COMPONENT: {
			bpp = 1;
			break;
		}
		default:
			THROW( "glTexImage2D unknown format " + std::to_string( format ) + " @" + source );
	}
	ASSERT( internalbpp == bpp,
		"glTexImage2D internal bpp (" + std::to_string( internalbpp ) + ") differs from bpp (" + std::to_string( bpp ) + ") @" + source
	);

	size_t size = bpp * width * height;
	alloc_t& old = m_opengl.textures.at( m_opengl.current_texture );
	if ( old.size > 0 ) {
		//Log( "Freeing " + std::to_string( size ) + " bytes from opengl texture " + std::to_string( m_opengl.current_texture ) + " @" + source );
		DEBUG_STAT_CHANGE_BY( opengl_textures_size, -old.size );
	}
	//Log( "Loading " + std::to_string( size ) + " bytes into opengl texture " + std::to_string( m_opengl.current_texture ) + " @" + source );

	m_opengl.textures[ m_opengl.current_texture ] = {
		size,
		source
	};
	DEBUG_STAT_CHANGE_BY( opengl_textures_size, size );
	DEBUG_STAT_INC( opengl_textures_updates );

	glTexImage2D_real( target, level, internalformat, width, height, border, format, type, pixels );
}

void MemoryWatcher::GLTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( target == GL_TEXTURE_2D, "glTexSubImage2D unknown target " + std::to_string( target ) + " @" + source );
	ASSERT( level == 0, "glTexSubImage2D unknown level " + std::to_string( level ) + " @" + source );
	ASSERT( width > 0, "glTexSubImage2D zero width" );
	ASSERT( height, "glTexSubImage2D zero height" );
	ASSERT( type == GL_UNSIGNED_BYTE, "glTexSubImage2D unknown type " + std::to_string( type ) + " @" + source );
	ASSERT( m_opengl.current_texture != 0, "glTexSubImage2D called without bound texture @" + source );

	size_t bpp;
	switch ( format ) {
		case GL_RED: {
			bpp = 1;
			break;
		}
		case GL_RGBA: {
			bpp = 4;
			break;
		}
		default:
			THROW( "glTexImage2D unknown format " + std::to_string( format ) + " @" + source );
	}

	DEBUG_STAT_INC( opengl_textures_updates );
	glTexSubImage2D_real( target, level, xoffset, yoffset, width, height, format, type, pixels );
}

void MemoryWatcher::GLDeleteTextures( GLsizei n, GLuint* textures, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( n == 1, "glDeleteTextures with size " + std::to_string( n ) + ", suspicious, is it a typo? @" + source );

	auto it = m_opengl.textures.find( *textures );
	ASSERT( it != m_opengl.textures.end(), "glDeleteTextures texture does not exist @" + source );

	ASSERT( m_opengl.current_texture != *textures, "glDeleteTextures destroying texture while it's still bound @" + source );

	if ( m_opengl.current_framebuffer ) {
		auto it = m_opengl.framebuffers.find( m_opengl.current_framebuffer );
		ASSERT( it != m_opengl.framebuffers.end(), "glDeleteTextures framebuffer not found @" + source );
		auto it2 = it->second.generated_textures.find( *textures );
		ASSERT( it2 != it->second.generated_textures.end(), "glDeleteTextures texture deleted within framebuffer scope but not generated within this framebuffer scope @" + source );
		it->second.generated_textures.erase( it2 );
	}

	auto it2 = m_opengl.textures_framebuffers.find( *textures );
	if ( it2 != m_opengl.textures_framebuffers.end() ) {
		ASSERT( it2->second == m_opengl.current_framebuffer, "glDeleteTextures deleting texture while owner framebuffer not bound @" + source );
		ASSERT( m_opengl.current_framebuffer_mode == FM_BIND, "glDeleteTextures deleting texture but owner framebuffer in wrong mode @" + source );
		auto it3 = m_opengl.framebuffers.find( m_opengl.current_framebuffer );
		ASSERT( it3 != m_opengl.framebuffers.end(), "glDeleteTextures framebuffer not found @" + source );
		if ( it3->second.textures.render == *textures ) {
			it3->second.textures.render = 0;
		}
		if ( it3->second.textures.depth == *textures ) {
			it3->second.textures.depth = 0;
		}
		m_opengl.textures_framebuffers.erase( it2 );
	}

	DEBUG_STAT_CHANGE_BY( opengl_textures_size, -it->second.size );
	DEBUG_STAT_DEC( opengl_textures_count );

	m_opengl.textures.erase( it );

	glDeleteTextures_real( n, textures );
}

void MemoryWatcher::GLGenFramebuffers( GLsizei n, GLuint* buffers, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( n == 1, "glGenFramebuffers with size " + std::to_string( n ) + ", suspicious, is it a typo? @" + source );

	glGenFramebuffers_real( n, buffers );
	//Log( "Created opengl buffer " + std::to_string( *buffers ) + " @" + source );

	ASSERT( m_opengl.framebuffers.find( *buffers ) == m_opengl.framebuffers.end(), "glGenFramebuffers buffer id overlap @" + source );
	m_opengl.framebuffers[ *buffers ] = {};

	DEBUG_STAT_INC( opengl_framebuffers_count );
}

void MemoryWatcher::GLBindFramebuffer( GLenum target, GLuint buffer, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER,
		"glBindFramebuffer unknown target " + std::to_string( target ) + " @" + source
	);

	ASSERT( !m_opengl.current_vertex_buffer, "glBindFramebuffer while vertex buffer is already bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glBindFramebuffer while index buffer is already bound @" + source );

	if ( buffer ) {
		auto it = m_opengl.framebuffers.find( buffer );
		ASSERT( it != m_opengl.framebuffers.end(), "glBindFramebuffer framebuffer " + std::to_string( buffer ) + " was not generated @" + source );
		ASSERT( m_opengl.current_framebuffer_mode == FM_NONE, "glBindFramebuffer op non-empty on bind @" + source );
		if ( it->second.textures.render ) {
			ASSERT( it->second.textures.render != m_opengl.current_texture, "glBindTexture tried to bind framebuffer but render texture is bound too @" + source );
		}
		if ( it->second.textures.depth ) {
			ASSERT( it->second.textures.depth != m_opengl.current_texture, "glBindTexture tried to bind framebuffer but depth texture is bound too @" + source );
		}
		m_opengl.current_framebuffer_mode =
			target == GL_FRAMEBUFFER
				? FM_BIND
				:
				target == GL_DRAW_FRAMEBUFFER
					? FM_DRAW
					:
					target == GL_READ_FRAMEBUFFER
						? FM_READ
						:
						FM_NONE;
	}
	else {
		ASSERT( m_opengl.current_framebuffer_mode != FM_NONE, "glBindFramebuffer op empty on unbind @" + source );
		m_opengl.current_framebuffer_mode = FM_NONE;
	}

	ASSERT( m_opengl.current_framebuffer != buffer,
		"glBindFramebuffer called with same framebuffer ( " + std::to_string( buffer ) + " ) as already bound @" + source
	);

	if ( buffer != 0 ) {
		ASSERT( m_opengl.current_framebuffer == 0,
			"glBindFramebuffer called on already bound framebuffer ( " + std::to_string( m_opengl.current_framebuffer ) + ", " + std::to_string( buffer ) + " ) @" + source
		);
		//Log("Binding to opengl framebuffer " + std::to_string( buffer ) + " @" + source );
	}
	else {
		if ( m_opengl.current_framebuffer != 0 ) {
			//Log("Unbinding from opengl framebuffer " + std::to_string( m_opengl.current_vertex_framebuffer ) + " @" + source );
		}
	}
	m_opengl.current_framebuffer = buffer;

	glBindFramebuffer_real( target, buffer );
}

void MemoryWatcher::GLFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( target == GL_FRAMEBUFFER,
		"glFramebufferTexture2D unknown target " + std::to_string( target ) + " @" + source
	);

	ASSERT( attachment == GL_COLOR_ATTACHMENT0 || attachment == GL_DEPTH_ATTACHMENT,
		"glFramebufferTexture2D unknown attachment " + std::to_string( attachment ) + " @" + source
	);

	ASSERT( textarget == GL_TEXTURE_2D,
		"glFramebufferTexture2D unknown textarget " + std::to_string( textarget ) + " @" + source
	);

	ASSERT( level == 0, "glFramebufferTexture2D level non-zero" );

	ASSERT( m_opengl.textures.find( texture ) != m_opengl.textures.end(), "glFramebufferTexture2D texture was not generated @" + source );
	ASSERT( m_opengl.current_framebuffer, "glFramebufferTexture2D no framebuffer bound @" + source );
	{
		auto it = m_opengl.textures_framebuffers.find( texture );
		ASSERT( it != m_opengl.textures_framebuffers.end() && it->second == m_opengl.current_framebuffer,
			"glFramebufferTexture2D not owner by bound framebuffer @" + source
		);
	}
	ASSERT( m_opengl.current_framebuffer_mode == FM_BIND, "glFramebufferTexture2D framebuffer in wrong mode @" + source );

	ASSERT( m_opengl.current_texture != texture, "glFramebufferTexture2D texture is already bound @" + source );

	auto it = m_opengl.framebuffers.find( m_opengl.current_framebuffer );
	ASSERT( it != m_opengl.framebuffers.end(), "glFramebufferTexture2D framebuffer not found @" + source );

	if ( attachment == GL_COLOR_ATTACHMENT0 ) {
		// TODO: fix ASSERT( !it->second.textures.render, "glFramebufferTexture2D framebuffer render texture already set @" + source );
		ASSERT( it->second.textures.depth != texture, "glFramebufferTexture2D texture can't be both render and depth @" + source );
		it->second.textures.render = texture;
	}
	else if ( attachment == GL_DEPTH_ATTACHMENT ) {
		// TODO: fix ASSERT( !it->second.textures.depth, "glFramebufferTexture2D framebuffer render texture already set @" + source );
		ASSERT( it->second.textures.render != texture, "glFramebufferTexture2D texture can't be both depth and render @" + source );
		it->second.textures.depth = texture;
	}

	glFramebufferTexture2D_real( target, attachment, textarget, texture, level );
}

void MemoryWatcher::GLDeleteFramebuffers( GLsizei n, const GLuint* buffers, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( n == 1, "glDeleteFramebuffers with size " + std::to_string( n ) + ", suspicious, is it a typo? @" + source );

	auto it = m_opengl.framebuffers.find( *buffers );
	ASSERT( it != m_opengl.framebuffers.end(), "glDeleteFramebuffers buffer does not exist @" + source );

	ASSERT( it->second.textures.render == 0, "glDeleteFramebuffers render texture still active @" + source );
	ASSERT( it->second.textures.depth == 0, "glDeleteFramebuffers depth texture still active @" + source );

	ASSERT( m_opengl.current_framebuffer != *buffers, "glDeleteFramebuffers destroying framebuffer while it's still bound @" + source );

	ASSERT( it->second.generated_buffers.empty(), "glDeleteFramebuffers framebuffer destroyed but some owned buffers still active @" + source );
	ASSERT( it->second.generated_textures.empty(), "glDeleteFramebuffers framebuffer destroyed but some owned textures still active @" + source );

	m_opengl.framebuffers.erase( it );

	DEBUG_STAT_DEC( opengl_framebuffers_count );

	glDeleteFramebuffers_real( n, buffers );
}

GLuint MemoryWatcher::GLCreateProgram( const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( !m_opengl.current_program, "glCreateProgram while some program is already active @" + source );
	ASSERT( !m_opengl.current_framebuffer, "glCreateProgram while some framebuffer is bound @" + source );
	ASSERT( !m_opengl.current_vertex_buffer, "glCreateProgram while some vertex buffer is bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glCreateProgram while some index buffer is bound @" + source );

	auto program = glCreateProgram_real();

	ASSERT( m_opengl.programs.find( program ) == m_opengl.programs.end(), "glCreateProgram program overlap @" + source );
	m_opengl.programs[ program ] = {};

	return program;
}

void MemoryWatcher::GLLinkProgram( GLuint program, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( !m_opengl.current_program, "glLinkProgram while some program is already active @" + source );
	ASSERT( !m_opengl.current_framebuffer, "glLinkProgram while some framebuffer is bound @" + source );
	ASSERT( !m_opengl.current_vertex_buffer, "glLinkProgram while some vertex buffer is bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glLinkProgram while some index buffer is bound @" + source );

	auto it = m_opengl.programs.find( program );
	ASSERT( it != m_opengl.programs.end(), "glLinkProgram program not found @" + source );

	ASSERT( !it->second.is_linked, "glLinkProgram program already linked @" + source );
	it->second.is_linked = true;

	glLinkProgram_real( program );
}

void MemoryWatcher::GLValidateProgram( GLuint program, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( !m_opengl.current_program, "glValidateProgram while some program is already active @" + source );
	ASSERT( !m_opengl.current_framebuffer, "glValidateProgram while some framebuffer is bound @" + source );
	ASSERT( !m_opengl.current_vertex_buffer, "glValidateProgram while some vertex buffer is bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glValidateProgram while some index buffer is bound @" + source );

	auto it = m_opengl.programs.find( program );
	ASSERT( it != m_opengl.programs.end(), "glValidateProgram program not found @" + source );

	ASSERT( it->second.is_linked, "glValidateProgram program not linked @" + source );
	ASSERT( !it->second.is_validated, "glValidateProgram program already validated @" + source );
	it->second.is_validated = true;

	glValidateProgram_real( program );
}

void MemoryWatcher::GLUseProgram( GLuint program, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	if ( program ) {
		ASSERT( !m_opengl.current_program, "glUseProgram starting but other program is already active @" + source );
		auto it = m_opengl.programs.find( program );
		ASSERT( it != m_opengl.programs.end(), "glUseProgram starting but program not found @" + source );
		ASSERT( it->second.is_validated, "glUseProgram starting but program not validated @" + source );
		m_opengl.current_program = program;
	}
	else {
		ASSERT( m_opengl.current_program, "glUseProgram stopping but no program is active @" + source );
		m_opengl.current_program = 0;
	}

	glUseProgram_real( program );
}

void MemoryWatcher::GLDeleteProgram( GLuint program, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( !m_opengl.current_program, "glDeleteProgram while some program is already active @" + source );
	ASSERT( !m_opengl.current_framebuffer, "glDeleteProgram while some framebuffer is bound @" + source );
	ASSERT( !m_opengl.current_vertex_buffer, "glDeleteProgram while some vertex buffer is bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glDeleteProgram while some index buffer is bound @" + source );

	auto it = m_opengl.programs.find( program );
	ASSERT( it != m_opengl.programs.end(), "glDeleteProgram program not found @" + source );

	m_opengl.programs.erase( it );

	glDeleteProgram_real( program );
}

void MemoryWatcher::GLDrawElements( GLenum mode, GLsizei count, GLenum type, const void* indices, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( mode == GL_QUADS || mode == GL_TRIANGLES, "glDrawElements unknown mode " + std::to_string( mode ) + " @" + source );
	ASSERT( type == GL_UNSIGNED_INT, "glDrawElements unknown type " + std::to_string( type ) + " @" + source );
	ASSERT( indices == nullptr, "glDrawElements indices non-null @" + source );
	ASSERT( m_opengl.current_vertex_buffer, "glDrawElements vertex buffer not bound @" + source );
	ASSERT( m_opengl.current_index_buffer, "glDrawElements index buffer not bound @" + source );
	ASSERT( m_opengl.current_program, "glDrawElements program not bound @" + source );

	const size_t bpi = 4; // bytes per index, 4 for unsigned int
	auto it = m_opengl.index_buffers.find( m_opengl.current_index_buffer );
	ASSERT( it != m_opengl.index_buffers.end(), "index buffer not found" );
	ASSERT( count * bpi == it->second.size,
		"glDrawElements count mismatch ( " + std::to_string( count * bpi ) + " " + std::to_string( it->second.size ) + " ) at index buffer " + std::to_string( m_opengl.current_index_buffer ) + " @" + source
	);

	DEBUG_STAT_INC( opengl_draw_calls );
	glDrawElements_real( mode, count, type, indices );
}

void MemoryWatcher::GLDrawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( mode == GL_QUADS || mode == GL_TRIANGLES, "glDrawElementsInstanced unknown mode " + std::to_string( mode ) + " @" + source );
	ASSERT( type == GL_UNSIGNED_INT, "glDrawElementsInstanced unknown type " + std::to_string( type ) + " @" + source );
	ASSERT( indices == nullptr, "glDrawElementsInstanced indices non-null @" + source );
	ASSERT( m_opengl.current_vertex_buffer, "glDrawElementsInstanced vertex buffer not bound @" + source );
	ASSERT( m_opengl.current_index_buffer, "glDrawElementsInstanced index buffer not bound @" + source );
	ASSERT( m_opengl.current_program, "glDrawElementsInstanced program not bound @" + source );

	const size_t bpi = 4; // bytes per index, 4 for unsigned int
	auto it = m_opengl.index_buffers.find( m_opengl.current_index_buffer );
	ASSERT( it != m_opengl.index_buffers.end(), "index buffer not found" );
	ASSERT( count * bpi == it->second.size,
		"glDrawElementsInstanced count mismatch ( " + std::to_string( count * bpi ) + " " + std::to_string( it->second.size ) + " ) at index buffer " + std::to_string( m_opengl.current_index_buffer ) + " @" + source
	);

	DEBUG_STAT_INC( opengl_draw_calls );
	glDrawElementsInstanced_real( mode, count, type, indices, primcount );
}

void MemoryWatcher::GLDrawArrays( GLenum mode, GLint first, GLsizei count, const std::string& file, const size_t line ) {
	std::lock_guard guard( m_mutex );
	const std::string source = file + ":" + std::to_string( line );

	ASSERT( mode == GL_TRIANGLE_STRIP, "glDrawArrays unknown mode " + std::to_string( mode ) + " @" + source );
	ASSERT( m_opengl.current_vertex_buffer, "glDrawArrays vertex buffer not bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glDrawArrays index buffer is bound but not supposed to be @" + source );
	ASSERT( m_opengl.current_program, "glDrawArrays program not bound @" + source );

	DEBUG_STAT_INC( opengl_draw_calls );
	glDrawArrays_real( mode, first, count );
}

struct sort_method {
	inline bool operator()( const MemoryWatcher::statistics_item_t& struct1, const MemoryWatcher::statistics_item_t& struct2 ) {
		return ( struct1.size > struct2.size );
	}
};

const MemoryWatcher::statistics_result_t MemoryWatcher::GetLargestMemoryConsumerClasses( size_t count ) {
	std::lock_guard guard( m_mutex );

	statistics_t stats;
	statistics_t::iterator it_dst;
	for ( auto& it_src : m_allocated_objects ) {
		const std::string& key = it_src.second.object_namespace;
		it_dst = stats.find( key );
		if ( it_dst == stats.end() ) {
			stats[ key ] = {
				0,
				0,
				key
			};
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

	std::sort( result.begin(), result.end(), sort_method() );

	if ( result.size() > count ) {
		result.resize( count );
	}

	return result;
}

void MemoryWatcher::Log( const std::string& text, const bool is_important ) {
	if ( !m_is_quiet || is_important ) {
		g_debug_stats._mutex.lock();
		if ( !g_debug_stats._readonly ) { // don't spam from debug overlay
			util::LogHelper::Println( "<MemoryWatcher> " + text );
			fflush( stdout );
		}
		g_debug_stats._mutex.unlock();
	}
}

}

#endif
