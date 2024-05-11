#pragma once

#include <functional>
#include <string>
#include <vector>

#include "base/Task.h"

namespace gse {
class GSE;
}

namespace task {
namespace gsetests {

typedef std::function< std::string( gse::GSE* gse ) > gse_test_t;
#define GT( ... ) [ __VA_ARGS__ ]( gse::GSE* gse ) -> std::string

#define GT_LOG( _text ) { \
    task->LogTest( _text );\
}

#define GT_OK() {\
    return "";\
}

#define GT_FAIL( _text ) {\
    return (std::string)__FILE__ + ":" + std::to_string(__LINE__) + ": " + ( _text );\
}

#define GT_ASSERT( _condition, ... ) {\
    if ( !( _condition ) ) {\
        GT_FAIL( (std::string) "assertion failed [ " # _condition " ]: " __VA_ARGS__ );\
    }\
}

CLASS( GSETests, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddTest( const std::string& name, const gse_test_t test );
	void LogTest( const std::string& text, bool is_debug = false );

private:
	size_t current_test_index = 0;
	std::vector< std::pair< std::string, gse_test_t >> m_tests = {};

	struct {
		size_t passed = 0;
		size_t failed = 0;
	} m_stats = {};
};

}
}
