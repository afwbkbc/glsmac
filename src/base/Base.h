#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>

#if DEBUG
#include <mutex>
#endif

using namespace std;

#if DEBUG
	
#define DEBUG_STATS \
	D( seconds_passed ) \
	D( objects_created ) \
	D( objects_destroyed ) \
	D( objects_active ) \
	D( frames_rendered ) \

#define D( _stat ) struct { \
		mutex stat_mutex; \
		ssize_t total = 0; \
		ssize_t current = 0; \
	} _stat;
	
	typedef struct {
		DEBUG_STATS
	} debug_stats_t;
	
#undef D
	
	extern debug_stats_t g_debug_stats;
	
	#define DEBUG_STAT_GET( _stat, _totalvar, _currentvar ) { \
		g_debug_stats._stat.stat_mutex.lock(); \
		_totalvar = g_debug_stats._stat.total; \
		_currentvar = g_debug_stats._stat.current; \
		g_debug_stats._stat.current = 0; \
		g_debug_stats._stat.stat_mutex.unlock(); \
	}

	#define DEBUG_STAT_INC( _stat ) { \
		g_debug_stats._stat.stat_mutex.lock(); \
		g_debug_stats._stat.total++; \
		g_debug_stats._stat.current++; \
		g_debug_stats._stat.stat_mutex.unlock(); \
	}

	#define DEBUG_STAT_DEC( _stat ) ( g_debug_stats._stat-- ) { \
		g_debug_stats._stat.stat_mutex.lock(); \
		g_debug_stats._stat.total--; \
		g_debug_stats._stat.current--; \
		g_debug_stats._stat.stat_mutex.unlock(); \
	}
	
#else
	#define DEBUG_STAT_INC( _stat )
	#define DEBUG_STAT_DEC( _stat )
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
