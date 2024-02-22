#pragma once

#ifdef DEBUG

#include <mutex>
#include <iostream>

#include "debug/MemoryWatcher.h"

using namespace debug;

#define DEBUG_STATS \
    D( seconds_passed ) \
    D( buffers_created ) \
    D( buffers_destroyed ) \
    D( buffers_active ) \
    D( objects_created ) \
    D( objects_destroyed ) \
    D( objects_active ) \
    D( heap_allocated_size ) \
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
    D( opengl_framebuffers_count ) \
    D( opengl_draw_calls ) \
    D( ui_elements_created ) \
    D( ui_elements_destroyed )\
    D( ui_elements_active )

#define D( _stat ) struct { \
        ssize_t total = 0; \
        ssize_t current = 0; \
    } _stat;

struct debug_stats_t {
	std::mutex _mutex; \
        bool _readonly = false;
	DEBUG_STATS
};

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
    g_debug_stats._mutex.unlock(); \
}

#define DEBUG_STAT_CLEAR( _stat ) { \
    g_debug_stats._mutex.lock(); \
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

#define malloc( __size ) g_memory_watcher->Malloc( __size, __FILE__, __LINE__ )
#define realloc( __ptr, __size ) g_memory_watcher->Realloc( __ptr, __size, __FILE__, __LINE__ )
#define ptr( _ptr, _offset, _size ) g_memory_watcher->Ptr( (unsigned char*)_ptr, _offset, _size, __FILE__, __LINE__ )
#define free( __ptr ) g_memory_watcher->Free( __ptr, __FILE__, __LINE__ )

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

#undef glGenFramebuffers
#define glGenFramebuffers( _size, _ptr ) g_memory_watcher->GLGenFramebuffers( _size, _ptr, __FILE__, __LINE__ )

#undef glBindFramebuffer
#define glBindFramebuffer( _target, _ptr ) g_memory_watcher->GLBindFramebuffer( _target, _ptr, __FILE__, __LINE__ )

#undef glFramebufferTexture2D
#define glFramebufferTexture2D( _target, _attachment, _textarget, _texture, _level ) g_memory_watcher->GLFramebufferTexture2D( _target, _attachment, _textarget, _texture, _level, __FILE__, __LINE__ )

#undef glDeleteFramebuffers
#define glDeleteFramebuffers( _size, _ptr ) g_memory_watcher->GLDeleteFramebuffers( _size, _ptr, __FILE__, __LINE__ )

#undef glCreateProgram
#define glCreateProgram() g_memory_watcher->GLCreateProgram( __FILE__, __LINE__ )

#undef glLinkProgram
#define glLinkProgram( _program ) g_memory_watcher->GLLinkProgram( _program, __FILE__, __LINE__ )

#undef glValidateProgram
#define glValidateProgram( _program ) g_memory_watcher->GLValidateProgram( _program, __FILE__, __LINE__ )

#undef glUseProgram
#define glUseProgram( _program ) g_memory_watcher->GLUseProgram( _program, __FILE__, __LINE__ )

#undef glDeleteProgram
#define glDeleteProgram( _program ) g_memory_watcher->GLDeleteProgram( _program, __FILE__, __LINE__ )

#undef glDrawElements
#define glDrawElements( _mode, _count, _type, _indices ) g_memory_watcher->GLDrawElements( _mode, _count, _type, _indices, __FILE__, __LINE__ )

#undef glDrawElementsInstanced
#define glDrawElementsInstanced( _mode, _count, _type, _indices, _primcount ) g_memory_watcher->GLDrawElementsInstanced( _mode, _count, _type, _indices, _primcount, __FILE__, __LINE__ )

#undef glDrawArrays
#define glDrawArrays( _mode, _first, _count ) g_memory_watcher->GLDrawArrays( _mode, _first, _count, __FILE__, __LINE__ )

#undef glDeleteTextures
#define glDeleteTextures( _size, _ptr ) g_memory_watcher->GLDeleteTextures( _size, _ptr, __FILE__, __LINE__ )

#define ASSERT( _condition, _text ) \
    if ( !( _condition ) ) { \
        Log( (std::string) "FATAL: " + _text ); \
        THROW( _text ); \
    }

// for rare cases when Log() is not available
#define ASSERT_NOLOG( _condition, _text ) \
    if ( !( _condition ) ) { \
        THROW( _text ); \
    }

#define TEST_OBJECT( _obj ) (_obj)->SetTesting( true )
#define UNTEST_OBJECT( _obj ) (_obj)->SetTesting( false )
#define TEST_BREAKPOINT( _obj ) (_obj)->TestBreakpoint()
#define IS_TESTING( _obj ) (_obj)->IsTesting()

#endif
