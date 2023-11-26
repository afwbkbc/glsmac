#pragma once

#include <functional>
#include <string>
#include <vector>

#include "base/Task.h"

#include "gse/GSE.h"

namespace task {
namespace gsetests {

typedef std::function< std::string( gse::GSE& gse ) > gse_test_t;
#define GT( ... ) [ __VA_ARGS__ ]( gse::GSE & gse ) -> std::string

#define GT_OK() {\
    return "";\
}

#define GT_FAIL( _text ) {\
    return (std::string)__FILE__ + ":" + std::to_string(__LINE__) + ": " + ( _text );\
}

#define GT_ASSERT( _condition, _text ) {\
    if ( !( _condition ) ) {\
        GT_FAIL( (std::string) "assertion failed [ " # _condition " ] " + ( _text ) );\
    }\
}

CLASS( GSETests, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddTest( const std::string& name, const gse_test_t test );

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
