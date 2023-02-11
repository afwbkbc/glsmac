#pragma once

#ifdef _WIN32
#ifdef VISUAL_STUDIO
#if !defined(ssize_t)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "env/Win32.h"
#endif

#include <string>
#include <stdexcept>

using namespace std;
#define THROW(  _text ) throw runtime_error( _text )

#ifdef DEBUG
#include "env/Debug.h"
#else
#include "env/Release.h"
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

#define CLASS( _name, _parent ) \
class _name : public _parent { \
public: \
	virtual const string GetNamespace() const { \
		return _parent::GetNamespace() + # _name + "::" ; \
	} \
	const string GetClassName() const { \
		return #_name; \
	}

} /* namespace base */

using namespace base;
