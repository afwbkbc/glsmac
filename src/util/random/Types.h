#pragma once

#include <cstdint>

namespace util {
namespace random {

typedef uint32_t value_t;

struct state_t {
	value_t a;
	value_t b;
	value_t c;
	value_t d;
};

}
}
