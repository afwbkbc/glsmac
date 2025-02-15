#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ui_legacy {
namespace object {

typedef size_t num_choice_t;
typedef std::string assoc_choice_t;
typedef std::vector< std::pair< num_choice_t, std::string > > num_choices_t;
typedef std::vector< std::pair< assoc_choice_t, std::string > > assoc_choices_t;

}
}
