#pragma once

#include <functional>

namespace ui_legacy {

typedef std::function< void() > ui_handler_t;
#define UH( ... ) [ __VA_ARGS__ ] () -> void

// return true if loaded should hide immediately
// return false if you want cancel button hidden but loader staying (you'll need to close loader manually later)
typedef std::function< bool() > loader_cancel_handler_t;
#define LCH( ... ) [ __VA_ARGS__ ] () -> bool

}
