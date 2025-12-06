#pragma once

#include <functional>
#include <unordered_map>

#include "common/Module.h"

#include "Event.h"

namespace input {

CLASS( Input, common::Module )

	typedef std::function< const bool( const Event& ) > input_handler_t;

	void AddHandler( const void* const owner, const input_handler_t& handler );
	void RemoveHandler( const void* const owner );

	virtual const std::string GetClipboardText() const = 0;

protected:
	const bool ProcessEvent( const Event& event );

private:
	std::unordered_map< const void*, input_handler_t > m_input_handlers = {};

};

}
