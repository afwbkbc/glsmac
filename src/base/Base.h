#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>

#if DEBUG
#include <mutex>
#include <iostream>
#include "debug/MemoryWatcher.h"
#endif

using namespace std;

#if DEBUG

using namespace debug;

#define DEBUG_STATS \
	D( seconds_passed ) \
	D( objects_created ) \
	D( objects_destroyed ) \
	D( objects_active ) \
	D( bytes_allocated ) \
	D( textures_loaded ) \
	D( fonts_loaded ) \
	D( frames_rendered ) \
	D( opengl_buffers_count ) \
	D( opengl_vertex_buffers_size ) \
	D( opengl_vertex_buffers_updates ) \
	D( opengl_index_buffers_size ) \
	D( opengl_index_buffers_updates ) \
	D( opengl_textures_count ) \
	D( opengl_textures_size ) \
	D( opengl_textures_updates ) \
	D( opengl_draw_calls ) \
	D( ui_elements_created ) \
	D( ui_elements_destroyed )\
	D( ui_elements_active )

#define D( _stat ) struct { \
		ssize_t total = 0; \
		ssize_t current = 0; \
	} _stat;
	
	typedef struct {
		mutex _mutex; \
		bool _readonly = false;
		DEBUG_STATS
	} debug_stats_t;
	
#undef D
	
	extern debug_stats_t g_debug_stats;
	
	// to prevent debug overlay from polluting stats by it's own activity
	#define DEBUG_STATS_SET_RO() { \
		g_debug_stats._mutex.lock(); \
		g_debug_stats._readonly = true; \
		g_debug_stats._mutex.unlock(); \
	}
	#define DEBUG_STATS_SET_RW() { \
		g_debug_stats._mutex.lock(); \
		g_debug_stats._readonly = false; \
		g_debug_stats._mutex.unlock(); \
	}

	#define DEBUG_STAT_GET( _stat, _totalvar, _currentvar ) { \
		g_debug_stats._mutex.lock(); \
		_totalvar = g_debug_stats._stat.total; \
		_currentvar = g_debug_stats._stat.current; \
		g_debug_stats._stat.current = 0; \
		g_debug_stats._mutex.unlock(); \
	}

	#define DEBUG_STAT_CHANGE_BY( _stat, _by ) { \
		g_debug_stats._mutex.lock(); \
		if ( !g_debug_stats._readonly ) { \
			g_debug_stats._stat.total += _by; \
			g_debug_stats._stat.current += _by; \
		} \
		g_debug_stats._mutex.unlock(); \
	}
	#define DEBUG_STAT_INC( _stat ) DEBUG_STAT_CHANGE_BY( _stat, 1 )
	#define DEBUG_STAT_DEC( _stat ) DEBUG_STAT_CHANGE_BY( _stat, -1 )
	
	#define NEW( _var, _class, ... ) \
		_var = new _class( __VA_ARGS__ ); \
		g_memory_watcher->New( _var, sizeof( _class ), __FILE__, __LINE__ );

	#define NEWV( _var, _class, ... ) \
		_class* _var; \
		NEW( _var, _class, __VA_ARGS__ )

	#define DELETE( _var ) \
		g_memory_watcher->Delete( _var, __FILE__, __LINE__ ); \
		delete _var;

	#undef glGenBuffers
	#define glGenBuffers( _size, _ptr ) g_memory_watcher->GLGenBuffers( _size, _ptr, __FILE__, __LINE__ )

	#undef glBindBuffer
	#define glBindBuffer( _target, _ptr ) g_memory_watcher->GLBindBuffer( _target, _ptr, __FILE__, __LINE__ )

	#undef glBufferData
	#define glBufferData( _target, _size, _data, _mode ) g_memory_watcher->GLBufferData( _target, _size, _data, _mode, __FILE__, __LINE__ )

	#undef glDeleteBuffers
	#define glDeleteBuffers( _size, _ptr ) g_memory_watcher->GLDeleteBuffers( _size, _ptr, __FILE__, __LINE__ )

	#undef glGenTextures
	#define glGenTextures( _size, _ptr ) g_memory_watcher->GLGenTextures( _size, _ptr, __FILE__, __LINE__ )

	#undef glBindTexture
	#define glBindTexture( _target, _ptr ) g_memory_watcher->GLBindTexture( _target, _ptr, __FILE__, __LINE__ )

	#undef glTexImage2D
	#define glTexImage2D( _target, _level, _srcfmt, _width, _height, _border, _dstfmt, _type, _ptr ) \
		g_memory_watcher->GLTexImage2D( _target, _level, _srcfmt, _width, _height, _border, _dstfmt, _type, _ptr, __FILE__, __LINE__ )
	
	#undef glTexSubImage2D
	#define glTexSubImage2D( _target, _level, _xoffset, _yoffset, _width, _height, format, _type, _ptr ) \
		g_memory_watcher->GLTexSubImage2D( _target, _level, _xoffset, _yoffset, _width, _height, format, _type, _ptr, __FILE__, __LINE__ )
	
	#undef glDrawElements
	#define glDrawElements( _mode, _count, _type, _indices ) g_memory_watcher->GLDrawElements( _mode, _count, _type, _indices, __FILE__, __LINE__ )

	#undef glDrawArrays
	#define glDrawArrays( _mode, _first, _count ) g_memory_watcher->GLDrawArrays( _mode, _first, _count, __FILE__, __LINE__ )
	
	#undef glDeleteTextures
	#define glDeleteTextures( _size, _ptr ) g_memory_watcher->GLDeleteTextures( _size, _ptr, __FILE__, __LINE__ )
	
#else
	#define DEBUG_STAT_CHANGE_BY( _stat, _by )
	#define DEBUG_STAT_INC( _stat )
	#define DEBUG_STAT_DEC( _stat )

	#define NEW( _var, _class, ... ) _var = new _class( __VA_ARGS__ )
	#define NEWV( _var, _class, ... ) auto* _var = new _class( __VA_ARGS__ )
	#define DELETE( _var ) delete _var
#endif
	
namespace base {

class Base {
public:
	Base();
	virtual ~Base();
	virtual const string GetNamespace() const;
	const string GetName() const;
	const string& GetLocalName() const;
	virtual const string GetClassName() const = 0;
protected:
	const size_t m_object_id = 0;
	string m_class_name = "";
	string m_name = "";
	void Log( const string &text ) const;
};

#define NAMED_CLASS( _name, _parent, _namespace ) \
class _name : public _parent { \
public: \
	virtual const string GetNamespace() const { \
		return _parent::GetNamespace() + # _namespace + "::" ; \
	} \
	const string GetClassName() const { \
		return #_name; \
	}

#define CHILD_CLASS( _name, _parent) \
NAMED_CLASS( _name, _parent, _name )

#define MINOR_CLASS( _name, _parent) \
NAMED_CLASS( _name ## _parent, _parent, _name )

#define MAJOR_CLASS( _name, _parent ) \
class _name ## Error : public base::Error { \
public:\
	_name ## Error(const string &reason) : base::Error::Error( reason ) {} \
	virtual const string GetNamespace() const { \
		return base::Error::GetNamespace() + # _name + "::" ; \
	} \
	const string GetClassName() const { \
		return #_name; \
	} \
}; \
NAMED_CLASS( _name, _parent, _name )

} /* namespace base */

#include "base/Error.h"
