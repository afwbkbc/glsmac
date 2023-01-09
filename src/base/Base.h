#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>

namespace base {

class Base {
public:
	virtual const std::string GetNamespace() const {
		return "XD";
	}
	virtual ~Base() {};
	const std::string GetName() const {
		if ( !m_name.empty() )
			return GetNamespace() + "::" + m_name;
		else
			return GetNamespace();
	}
	const std::string & GetLocalName() const {
		return m_name;
	}
protected:
	std::string m_name;
	void Log( const std::string &text ) const;
};

#define NAMED_CLASS( _name, _parent, _namespace ) \
class _name : public _parent {\
public:\
	virtual const std::string GetNamespace() const {\
		return _parent::GetNamespace() + "::" # _namespace;\
	}

#define CHILD_CLASS( _name, _parent) \
NAMED_CLASS( _name, _parent, _name )

#define MINOR_CLASS( _name, _parent) \
NAMED_CLASS( _name ## _parent, _parent, _name )

#define MAJOR_CLASS( _name, _parent ) \
class _name ## Error : public base::Error {\
public:\
	_name ## Error(const std::string &reason) : base::Error::Error( reason ) {};\
	virtual const std::string GetNamespace() const {\
		return base::Error::GetNamespace() + "::" # _name;\
	}\
};\
NAMED_CLASS( _name, _parent, _name )

} /* namespace base */

#include "base/Error.h"
