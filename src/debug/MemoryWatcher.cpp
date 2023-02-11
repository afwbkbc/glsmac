#ifdef DEBUG

#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstring>

#include "MemoryWatcher.h"

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
void glGenTextures_real( GLsizei n, GLuint * textures ) {
	glGenTextures( n, textures );
}
void glBindTexture_real( GLenum target, GLuint texture ) {
	glBindTexture( target, texture );
}
void glTexImage2D_real( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
	glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
}
void glTexSubImage2D_real( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels ) {
	glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
}
void glDeleteTextures_real( GLsizei n, GLuint * textures ) {
	glDeleteTextures( n, textures );
}
void glDrawElements_real( GLenum mode, GLsizei count, GLenum type, const void * indices ) {
	glDrawElements( mode, count, type, indices );
}

void glDrawArrays_real( GLenum mode, GLint first, GLsizei count ) {
	glDrawArrays( mode, first, count );
}

#include "base/Base.h"

namespace debug {

MemoryWatcher* g_memory_watcher = nullptr;

MemoryWatcher::MemoryWatcher() {
	ASSERT( !g_memory_watcher, "duplicate MemoryWatcher instantiation" );
	g_memory_watcher = this;
}

MemoryWatcher::~MemoryWatcher() {
#ifdef MEMORY_DEBUG
	
	Log( "Checking for possible memory leaks..." );
	
	bool any_leaks = false;
#define CHECK_LEAKS( _where ) \
	if ( !_where.empty() ) { \
		Log( "WARNING: " + std::to_string( _where.size() ) + " objects were never freed (possible memory leaks?):" ); \
		for (auto& o : _where) { \
			std::stringstream ptrstr; \
			ptrstr << o.first; \
			Log( "    (" + ptrstr.str() + ") @" + o.second.source ); \
		} \
		any_leaks = true; \
	}
	CHECK_LEAKS( m_allocated_objects )
	CHECK_LEAKS( m_allocated_memory )
	CHECK_LEAKS( m_opengl.vertex_buffers )
	CHECK_LEAKS( m_opengl.index_buffers )
	CHECK_LEAKS( m_opengl.textures )
	
#undef CHECK_LEAKS
			
	if ( !any_leaks ) {
		Log( "No memory leaks detected." );
	}
#endif
}

void MemoryWatcher::New( const Base* object, const size_t size, const std::string& file, const size_t line ) {
#ifdef MEMORY_DEBUG
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);

	ASSERT( m_allocated_objects.find( object ) == m_allocated_objects.end(), "new double-allocation detected @" + source );
	m_allocated_objects[ object ] = {
		object,
		size,
		object->GetName(),
		object->GetClassName(),
		source,
	};
	
	DEBUG_STAT_INC( objects_created );
	DEBUG_STAT_INC( objects_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, size );

	// VERY spammy
	//Log( "Allocated " + to_string( size ) + "b for " + object->GetNamespace() + " @" + source );
#endif
}

void MemoryWatcher::Delete( const Base* object, const std::string& file, const size_t line ) {
#ifdef MEMORY_DEBUG
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);

	auto it = m_allocated_objects.find( object );
	ASSERT( it != m_allocated_objects.end(), "delete on non-allocated object detected @" + source );
	
	auto& obj = it->second;
	
	DEBUG_STAT_INC( objects_destroyed );
	DEBUG_STAT_DEC( objects_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );

	// VERY spammy
	//Log( "Freed " + to_string( obj.size ) + "b from " + object->GetNamespace() + " @" + source );
	
	m_allocated_objects.erase( it );
#endif
}

void* MemoryWatcher::Malloc( const size_t size, const std::string& file, const size_t line ) {
#ifdef MEMORY_DEBUG
	std::lock_guard< std::mutex> guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( size > 0, "allocation of size 0 @" + source );
	
#endif
	void *ptr = malloc_real( size );
#ifdef MEMORY_DEBUG	
	ASSERT( m_allocated_memory.find( ptr ) == m_allocated_memory.end(), "malloc double-allocation detected @" + source );
	
	m_allocated_memory[ ptr ] = {
		size,
		source
	};
	
	DEBUG_STAT_INC( buffers_created );
	DEBUG_STAT_INC( buffers_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, size );
	
	// VERY spammy
	//Log( "Allocated " + to_string( size ) + "b for " + to_string( (long int)ptr ) + " @" + source );
#endif	
	return ptr;
}

void* MemoryWatcher::Realloc( void* ptr, const size_t size, const std::string& file, const size_t line ) {
#ifdef MEMORY_DEBUG
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( ptr, "reallocation of null @" + source );
	
	ASSERT( size > 0, "reallocation of size 0 @" + source );
	
	auto it = m_allocated_memory.find( ptr );
	ASSERT( it != m_allocated_memory.end(), "realloc on non-allocated object detected @" + source );
	
	auto& obj = it->second;
	
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );
	
	// VERY spammy
	//Log( "Freed " + to_string( obj.size ) + "b from " + to_string( (long int)ptr ) + " @" + source );
	
	m_allocated_memory.erase( it );
#endif	
	ptr = realloc_real( ptr, size );
#ifdef MEMORY_DEBUG	
	ASSERT( m_allocated_memory.find( ptr ) == m_allocated_memory.end(), "realloc double-allocation detected @" + source );
	
	m_allocated_memory[ ptr ] = {
		size,
		source
	};
	
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, size );
	
	// VERY spammy
	//Log( "Allocated " + to_string( size ) + "b for " + to_string( (long int)ptr ) + " @" + source );
#endif
	return ptr;
}

unsigned char* MemoryWatcher::Ptr( unsigned char* ptr, const size_t offset, const size_t size, const std::string& file, const size_t line  ) {
#ifdef MEMORY_DEBUG
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( ptr, "ptr is null @" + source );
	
	auto it = m_allocated_memory.find( ptr );
	ASSERT( it != m_allocated_memory.end(), "ptr on non-allocated pointer @" + source );
	
	ASSERT( offset + size <= it->second.size,
		"ptr overflow (" + std::to_string( offset ) + " + " + std::to_string( size ) + " > " + std::to_string( it->second.size ) + ") @" + source + " (allocated @" + it->second.source + ")"
	);
#endif	
	return ptr + offset;
}

void MemoryWatcher::Free( void* ptr, const std::string& file, const size_t line ) {
#ifdef MEMORY_DEBUG
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	auto it = m_allocated_memory.find( ptr );
	ASSERT( it != m_allocated_memory.end(), "free on non-allocated object " + std::to_string( (long int)ptr ) + " detected @" + source );
	
	auto& obj = it->second;
#endif	
	free_real( ptr );
#ifdef MEMORY_DEBUG	
	DEBUG_STAT_INC( buffers_destroyed );
	DEBUG_STAT_DEC( buffers_active );
	DEBUG_STAT_CHANGE_BY( heap_allocated_size, -obj.size );
	
	// VERY spammy
	//Log( "Freed " + to_string( obj.size ) + "b from " + to_string( (long int)ptr ) + " @" + source );
	
	m_allocated_memory.erase( it );
#endif
}
	
void MemoryWatcher::GLGenBuffers( GLsizei n, GLuint * buffers, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( n == 1, "glGenBuffers with size " + std::to_string(n) + ", suspicious, is it a typo? @" + source );
	
	glGenBuffers_real( n, buffers );
	//Log( "Created opengl buffer " + to_string( *buffers ) + " @" + source );
	
	ASSERT( m_opengl.buffers.find( *buffers ) == m_opengl.buffers.end(), "glGenBuffers buffer id overlap @" + source );
	
	m_opengl.buffers.insert( *buffers );
	
	DEBUG_STAT_INC( opengl_buffers_count );
}

void MemoryWatcher::GLBindBuffer( GLenum target, GLuint buffer, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER,
		"glBindBuffer unknown target " + std::to_string(target) + " @" + source
	);

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
				m_opengl.vertex_buffers[ buffer ] = { 0, source };
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
				m_opengl.index_buffers[ buffer ] = { 0, source };
			}
			//Log("Binding to opengl index buffer " + to_string( buffer ) + " @" + source );
		}
		else {
			if ( m_opengl.current_index_buffer != 0 ) {
				//Log("Unbinding from opengl index buffer " + to_string( m_opengl.current_index_buffer  ) + " @" + source );
			}
		}
		m_opengl.current_index_buffer = buffer;
	}
	
	glBindBuffer_real( target, buffer );
}

void MemoryWatcher::GLBufferData( GLenum target, GLsizeiptr size, const void * data, GLenum usage, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT(
		target == GL_ARRAY_BUFFER ||
		target == GL_ELEMENT_ARRAY_BUFFER,
		"glBufferData unknown target " + std::to_string(target)
	);
	
	if ( target == GL_ARRAY_BUFFER ) {
		ASSERT( m_opengl.current_vertex_buffer != 0, "glBufferData called without bound vertex buffer @" + source );
		
		auto it = m_opengl.vertex_buffers.find( m_opengl.current_vertex_buffer );
		ASSERT( it != m_opengl.vertex_buffers.end(), "opengl vertex buffer not bound" );
		if ( it->second.size > 0 ) {
			//Log( "Freeing " + to_string( size ) + " bytes from opengl vertex buffer " + to_string( m_opengl.current_vertex_buffer ) + " @" + source );
			DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, -it->second.size );
		}
		//Log( "Loading " + to_string( size ) + " bytes into opengl vertex buffer " + to_string( m_opengl.current_vertex_buffer ) + " @" + source );
		it->second.size = (size_t)size;
		DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, size );
		DEBUG_STAT_INC( opengl_vertex_buffers_updates );
	}
	else {
		ASSERT( m_opengl.current_index_buffer != 0, "glBufferData called without bound index buffer @" + source );
		
		auto it = m_opengl.index_buffers.find( m_opengl.current_index_buffer );
		ASSERT( it != m_opengl.index_buffers.end(), "opengl index buffer not bound" );
		if ( it->second.size > 0 ) {
			//Log( "Freeing " + to_string( size ) + " bytes from opengl index buffer " + to_string( m_opengl.current_index_buffer ) + " @" + source );
			DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, -it->second.size );
		}
		//Log( "Loading " + to_string( size ) + " bytes into opengl index buffer " + to_string( m_opengl.current_index_buffer ) + " @" + source );
		it->second.size = (size_t)size;
		DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, size );
		DEBUG_STAT_INC( opengl_index_buffers_updates );
	}
	
	glBufferData_real( target, size, data, usage );
}

void MemoryWatcher::GLDeleteBuffers( GLsizei n, const GLuint * buffers, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( n == 1, "glDeleteBuffers with size " + std::to_string(n) + ", suspicious, is it a typo? @" + source );
	
	ASSERT( m_opengl.buffers.find( *buffers ) != m_opengl.buffers.end(), "glDeleteBuffers buffer does not exist @" + source );
	
	auto it_vertex = m_opengl.vertex_buffers.find( *buffers );
	auto it_index = m_opengl.index_buffers.find( *buffers );
	
	ASSERT( it_vertex == m_opengl.vertex_buffers.end() || it_index == m_opengl.index_buffers.end(),
		"glDeleteBuffers buffer is both vertex buffer and index buffer, is it a bug? @" + source
	);
	if ( it_vertex != m_opengl.vertex_buffers.end() ) {
		//Log( "Destroying opengl vertex buffer " + to_string( *buffers ) + " @" + source );
		DEBUG_STAT_CHANGE_BY( opengl_vertex_buffers_size, -it_vertex->second.size );
		m_opengl.vertex_buffers.erase( it_vertex );
	}
	if ( it_index != m_opengl.index_buffers.end() ) {
		//Log( "Destroying opengl index buffer " + to_string( *buffers ) + " @" + source );
		DEBUG_STAT_CHANGE_BY( opengl_index_buffers_size, -it_index->second.size );
		m_opengl.index_buffers.erase( it_index );
	}
	
	m_opengl.buffers.erase( *buffers );
	DEBUG_STAT_DEC( opengl_buffers_count );
	
	glDeleteBuffers_real( n, buffers );
}

void MemoryWatcher::GLGenTextures( GLsizei n, GLuint * textures, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( n == 1, "glGenTextures with size " + std::to_string(n) + ", suspicious, is it a typo? @" + source );
	
	glGenTextures_real( n, textures );
	//Log( "Created opengl texture " + to_string( *textures ) + " @" + source );
	
	ASSERT( m_opengl.textures.find( *textures ) == m_opengl.textures.end(), "glGenTextures texture id overlap @" + source );
	
	m_opengl.textures[ *textures ] = { 0, source };
	
	DEBUG_STAT_INC( opengl_textures_count );
}

void MemoryWatcher::GLBindTexture( GLenum target, GLuint texture, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( target == GL_TEXTURE_2D, "glBindTexture unknown target " + std::to_string(target) + " @" + source );

	ASSERT( m_opengl.current_texture != texture, "glBindTexture called with same texture ( " + std::to_string( texture ) + " ) as already bound @" + source );
	if ( texture != 0 ) {
		ASSERT( m_opengl.current_texture == 0, "glBindTexture called on already bound texture ( " + std::to_string( m_opengl.current_texture ) + ", " + std::to_string( texture ) + " ) @" + source );
		//Log("Binding to opengl texture " + to_string( texture ) + " @" + source );
		if ( m_opengl.textures.find( texture ) == m_opengl.textures.end() ) {
			m_opengl.textures[ texture ] = { 0, source };
		}
	}
	else {
		if ( m_opengl.current_texture != 0 ) {
			//Log("Unbinding from opengl texture " + to_string( m_opengl.current_vertex_buffer ) + " @" + source );
		}
	}
	m_opengl.current_texture = texture;
	
	glBindTexture_real( target, texture );
}

void MemoryWatcher::GLTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( target == GL_TEXTURE_2D, "glTexImage2D unknown target " + std::to_string(target) + " @" + source );
	ASSERT( level == 0, "glTexImage2D unknown level " + std::to_string(level) + " @" + source );
	ASSERT( width > 0, "glTexImage2D zero width" );
	ASSERT( height > 0, "glTexImage2D zero height" );
	ASSERT( border == 0, "glTexImage2D unknown border " + std::to_string(border) + " @" + source );
	ASSERT(
		type == GL_UNSIGNED_BYTE ||
		type == GL_UNSIGNED_INT ||
		type == GL_FLOAT,
		"glTexImage2D unknown type " + std::to_string(type) + " @" + source
	);
	ASSERT( m_opengl.current_texture != 0, "glTexImage2D called without bound texture @" + source );
	
	size_t internalbpp, bpp;
	switch (internalformat) {
		case GL_RED: {
			internalbpp = 1;
			break;
		}
		case GL_RGBA8: {
			internalbpp = 4;
			break;
		}
		case GL_RGB32UI: {
			internalbpp = 4;
			break;
		}
		case GL_DEPTH_COMPONENT: {
			internalbpp = 1;
			break;
		}
		default:
			ASSERT( false, "glTexImage2D unknown internal format " + std::to_string(internalformat) + " @" + source );
	}
	switch (format) {
		case GL_RED: {
			bpp = 1;
			break;
		}
		case GL_RGBA: {
			bpp = 4;
			break;
		}
		case GL_RGB_INTEGER: {
			bpp = 4;
			break;
		}
		case GL_DEPTH_COMPONENT: {
			bpp = 1;
			break;
		}
		default:
			ASSERT( false, "glTexImage2D unknown format " + std::to_string(format) + " @" + source );
	}
	ASSERT( internalbpp == bpp,
		"glTexImage2D internal bpp (" + std::to_string( internalbpp ) + ") differs from bpp (" + std::to_string( bpp ) + ") @" + source
	);
	
	size_t size = bpp * width * height;
	alloc_t& old = m_opengl.textures.at( m_opengl.current_texture );
	if ( old.size > 0 ) {
		//Log( "Freeing " + to_string( size ) + " bytes from opengl texture " + to_string( m_opengl.current_texture ) + " @" + source );
		DEBUG_STAT_CHANGE_BY( opengl_textures_size, -old.size );
	}
	//Log( "Loading " + to_string( size ) + " bytes into opengl texture " + to_string( m_opengl.current_texture ) + " @" + source );
	
	m_opengl.textures[ m_opengl.current_texture ] = { size, source };
	DEBUG_STAT_CHANGE_BY( opengl_textures_size, size );
	DEBUG_STAT_INC( opengl_textures_updates );
	
	glTexImage2D_real( target, level, internalformat, width, height, border, format, type, pixels );
}

void MemoryWatcher::GLTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( target == GL_TEXTURE_2D, "glTexSubImage2D unknown target " + std::to_string(target) + " @" + source );
	ASSERT( level == 0, "glTexSubImage2D unknown level " + std::to_string(level) + " @" + source );
	ASSERT( width > 0, "glTexSubImage2D zero width" );
	ASSERT( height, "glTexSubImage2D zero height" );
	ASSERT( type == GL_UNSIGNED_BYTE, "glTexSubImage2D unknown type " + std::to_string(type) + " @" + source );
	ASSERT( m_opengl.current_texture != 0, "glTexSubImage2D called without bound texture @" + source );
	
	size_t bpp;
	switch (format) {
		case GL_RED: {
			bpp = 1;
			break;
		}
		case GL_RGBA: {
			bpp = 4;
			break;
		}
		default:
			ASSERT( false, "glTexImage2D unknown format " + std::to_string(format) + " @" + source );
	}
	
	DEBUG_STAT_INC( opengl_textures_updates );
	glTexSubImage2D_real( target, level, xoffset, yoffset, width, height, format, type, pixels );
}

void MemoryWatcher::GLDeleteTextures( GLsizei n, GLuint * textures, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( n == 1, "glDeleteTextures with size " + std::to_string(n) + ", suspicious, is it a typo? @" + source );
	
	auto it = m_opengl.textures.find( *textures );
	ASSERT( it != m_opengl.textures.end(), "glDeleteTextures texture does not exist @" + source );
	
	DEBUG_STAT_CHANGE_BY( opengl_textures_size, -it->second.size );
	DEBUG_STAT_DEC( opengl_textures_count );
	
	m_opengl.textures.erase( it );
	
	glDeleteTextures_real( n, textures );
}

void MemoryWatcher::GLDrawElements( GLenum mode, GLsizei count, GLenum type, const void * indices, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( mode == GL_QUADS || mode == GL_TRIANGLES, "glDrawElements unknown mode " + std::to_string( mode ) + " @" + source );
	ASSERT( type == GL_UNSIGNED_INT, "glDrawElements unknown type " + std::to_string( type ) + " @" + source );
	ASSERT( indices == nullptr, "glDrawElements indices non-null @" + source );
	ASSERT( m_opengl.current_vertex_buffer, "glDrawElements vertex buffer not bound @" + source );
	ASSERT( m_opengl.current_index_buffer, "glDrawElements index buffer not bound @" + source );
	
	const size_t bpi = 4; // bytes per index, 4 for unsigned int
	auto it = m_opengl.index_buffers.find( m_opengl.current_index_buffer );
	ASSERT( it != m_opengl.index_buffers.end(), "index buffer not found" );
	ASSERT( count * bpi == it->second.size,
		"glDrawElements count mismatch ( " + std::to_string( count* bpi ) + " " + std::to_string( it->second.size ) + " ) at index buffer " + std::to_string( m_opengl.current_index_buffer ) + " @" + source
	);
	
	DEBUG_STAT_INC( opengl_draw_calls );
	glDrawElements_real( mode, count, type, indices );
}

void MemoryWatcher::GLDrawArrays( GLenum mode, GLint first, GLsizei count, const std::string& file, const size_t line ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	const std::string source = file + ":" + std::to_string(line);
	
	ASSERT( mode == GL_TRIANGLE_STRIP, "glDrawArrays unknown mode " + std::to_string( mode ) + " @" + source );
	ASSERT( m_opengl.current_vertex_buffer, "glDrawArrays vertex buffer not bound @" + source );
	ASSERT( !m_opengl.current_index_buffer, "glDrawArrays index buffer is bound but not supposed to be @" + source );
	
	DEBUG_STAT_INC( opengl_draw_calls );
	glDrawArrays_real( mode, first, count );
}

struct sort_method
{
    inline bool operator() (const MemoryWatcher::statistics_item_t& struct1, const MemoryWatcher::statistics_item_t& struct2)
    {
        return (struct1.size > struct2.size);
    }
};

const MemoryWatcher::statistics_result_t MemoryWatcher::GetLargestMemoryConsumerClasses( size_t count ) {
	std::lock_guard< std::mutex > guard( m_mutex );
	
	statistics_t stats;
	statistics_t::iterator it_dst;
	for ( auto& it_src : m_allocated_objects ) {
		const std::string& key = it_src.second.object_namespace;
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
	
	std::sort( result.begin(), result.end(), sort_method() );
	
	if ( result.size() > count ) {
		result.resize( count );
	}
	
	return result;
}

void MemoryWatcher::Log( const std::string& text ) {
	g_debug_stats._mutex.lock();
	if (!g_debug_stats._readonly) { // don't spam from debug overlay
		std::cout << "<MemoryWatcher> " << text << std::endl;
		fflush(stdout);
	}
	g_debug_stats._mutex.unlock();
}

}

#endif
