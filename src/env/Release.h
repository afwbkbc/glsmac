#pragma once

#ifndef DEBUG

#define DEBUG_STAT_CHANGE_BY( _stat, _by )
#define DEBUG_STAT_INC( _stat )
#define DEBUG_STAT_DEC( _stat )

#define NEW( _var, _class, ... ) _var = new _class( __VA_ARGS__ )
#define NEWV( _var, _class, ... ) auto* _var = new _class( __VA_ARGS__ )
#define DELETE( _var ) delete _var
#define ptr( _ptr, _offset, _size ) ( _ptr + (_offset) )

#define TEST_OBJECT( _obj )
#define UNTEST_OBJECT( _obj )
#define TEST_BREAKPOINT( _obj )
#define IS_TESTING( _obj ) false

#endif

