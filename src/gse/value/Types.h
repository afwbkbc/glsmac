#pragma once

#include <string>
#include <map>
#include <vector>

#include "gse/Value.h"

namespace gse {
namespace value {

typedef std::string object_key_t; // keep it simple for now
typedef std::map< object_key_t, Value* > object_properties_t;

typedef std::vector< Value* > array_elements_t;

typedef std::vector< Value* > function_arguments_t;

}
}
