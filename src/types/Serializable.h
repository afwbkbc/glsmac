#pragma once

#include "base/Base.h"

#include "Buffer.h"

namespace types {

// don't set namespace
class Serializable : public base::Base {
public:
	
	virtual const Buffer Serialize() const = 0;
	virtual void Unserialize( Buffer buffer ) = 0;
	
	virtual void operator= ( const Serializable& other ) {
		// not super efficient, but convenient
		Unserialize( other.Serialize() );
	}
	
};

}
