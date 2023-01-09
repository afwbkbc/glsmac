#pragma once

#include <exception>
#include <string>
#include <cstring>

#include "Base.h"

namespace base {

static char gLastExceptionMsg[1000];

class Error : public std::exception, public Base {
public:
	Error(const std::string &reason) {
		this->mReason = reason;
	}
	virtual const std::string GetNamespace() const {
		return Base::GetNamespace() + "::Error";
	}
	virtual const char* what() const throw() {
		std::string message = "<" + GetName() + "> " + this->mReason;
		size_t sz = message.size();
		if ( sz > sizeof( gLastExceptionMsg ) - 1 )
			sz = sizeof( gLastExceptionMsg ) - 1;
		memcpy( &gLastExceptionMsg, message.c_str(), sz );
		gLastExceptionMsg[sz] = '\0';
		return gLastExceptionMsg;
	}
protected:
	std::string mReason;
};

} /* namespace base */
