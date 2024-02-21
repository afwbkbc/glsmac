#pragma once

#ifdef _WIN32
#include "env/Win32.h"
#else
#include "env/Posix.h"
#endif

#include <string>
#include <stdexcept>

#define THROW( _text ) throw std::runtime_error( _text )

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

	virtual const std::string GetNamespace() const;
	const std::string GetName() const;
	const std::string& GetLocalName() const;
	virtual const std::string GetClassName() const = 0;

#ifdef DEBUG
	// use these to track lifecycle of specific object
	void SetTesting( const bool testing );
	const bool IsTesting() const;
	void TestBreakpoint(); // set gdb breakpoint inside
#endif

protected:
	size_t m_object_id = 0;
	std::string m_class_name = "";
	std::string m_name = "";

	void Log( const std::string& text ) const;

private:

#ifdef DEBUG
	bool m_is_testing = false;
#endif

};

#define CLASS_HEADER( _name, _parent ) \
public: \
    virtual const std::string GetNamespace() const override { \
        return _parent::GetNamespace() + # _name + "::" ; \
    } \
    const std::string GetClassName() const override { \
        return #_name; \
    }

#define CLASS( _name, _parent ) \
class _name : public _parent { \
    CLASS_HEADER( _name, _parent )

#define CLASS2( _name, _parent1, _parent2 ) \
class _name : public _parent1, public _parent2 { \
    CLASS_HEADER( _name, _parent1 )

} /* namespace base */

using namespace base;
