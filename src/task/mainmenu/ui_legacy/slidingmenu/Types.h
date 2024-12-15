#pragma once

#include <functional>

namespace task {
namespace mainmenu {

typedef std::function< void() > choice_handler_t;
struct choice_handlers_t {
	choice_handler_t on_click = 0;
	choice_handler_t on_select = 0;
};
typedef std::vector< std::pair< std::string, choice_handlers_t > > choices_t;

}
}
