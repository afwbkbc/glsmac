#pragma once

#include <string>
#include <cstdlib>

namespace gse {

// source info, this is how source elements (and later programs) are linked to source code
struct si_t {
	std::string file = "";
	struct pos_t {
		size_t line = 0;
		size_t col = 0;
	};
	pos_t from = {};
	pos_t to = {};
	const std::string ToString() const {
		return "[" + file + ":" + std::to_string( from.line ) + ":" + std::to_string( from.col ) + "]";
	}
};

class ExecutionPointer;

}
