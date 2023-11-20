#pragma once

#include <functional>
#include <string>
#include <vector>

#include "base/Task.h"

namespace task {
namespace worldtests {

typedef std::function< std::string() > world_test_t;
#define WT( ... ) [ __VA_ARGS__ ]() -> std::string

CLASS( WorldTests, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddTest( const std::string& name, const world_test_t test );

private:
	size_t current_test_index = 0;
	std::vector< std::pair< std::string, world_test_t >> m_tests = {};
};

}
}
