#pragma once

#ifdef _WIN32
#include "env/Win32.h"
#else
#include "env/Posix.h"
#endif

#include <string>

#include "Assert.h"

#ifdef DEBUG
#include "env/Debug.h"
#else
#include "env/Release.h"
#endif

// used in multiline ToString() implementations

#define TS_DEF() const std::string ToString( const std::string& prefix ) const override;
#define TS_BEGIN( _class ) const std::string _class::ToString( const std::string& prefix ) const { \
    return (std::string)
#define TS_END() }

#define TS_OFFSET "    "

#define TS_PREFIX prefix
#define TS_PREFIX_NEXT TS_PREFIX + TS_OFFSET

#define TS_ML_FIRST( _text ) _text + "\n"
#define TS_ML_NEXT( _text ) TS_PREFIX_NEXT + _text + "\n"
#define TS_ML_LAST( _text ) TS_PREFIX + _text

#define TS_ARR_BEGIN( _class ) TS_ML_FIRST( (std::string) _class + "( [" )
#define TS_ARR_EL( _value ) TS_ML_NEXT( _value )
#define TS_ARR_EL_STR( _value ) TS_ARR_EL( "'" + _value + "'" )
#define TS_ARR_EL_NUM( _value ) TS_ARR_EL( std::to_string( _value ) )
#define TS_ARR_END() TS_ML_LAST( "] )" )

#define TS_OBJ_BEGIN( _class ) TS_ML_FIRST( (std::string) _class + "( {" )
#define TS_OBJ_PROP( _key, _value ) TS_ML_NEXT( _key + ": " + _value )
#define TS_OBJ_PROP_STR( _key, _value ) TS_OBJ_PROP( _key, "'" + _value + "'" )
#define TS_OBJ_PROP_NUM( _key, _value ) TS_OBJ_PROP( _key, std::to_string( _value ) )
#define TS_OBJ_END() TS_ML_LAST( "} )" )

#define TS_FUNC_BEGIN( _funcname ) TS_ML_FIRST( (std::string) _funcname + "( " )
#define TS_FUNC_ARG( _name, _value ) TS_ML_NEXT( _name + " = " + _value )
#define TS_FUNC_ARG_STR( _name, _value ) TS_ML_NEXT( _name + " = '" + _value + "'" )
#define TS_FUNC_ARG_NUM( _name, _value ) TS_ML_NEXT( _name + " = " + std::to_string( _value ) )
#define TS_FUNC_END() TS_ML_LAST( " )" );

#define TS_OF( _what ) ( _what )->ToString( TS_PREFIX_NEXT )

namespace common {

class Class {
public:
	Class();
	virtual ~Class() = default;

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

#define CLASS3( _name, _parent1, _parent2, _parent3 ) \
class _name : public _parent1, public _parent2, public _parent3 { \
    CLASS_HEADER( _name, _parent1 )

}
