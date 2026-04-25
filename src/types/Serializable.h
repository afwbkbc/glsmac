#pragma once

#include "common/Common.h"

#include "Buffer.h"

namespace types {

// don't set namespace
class Serializable : public common::Class {
public:

	virtual ~Serializable() = default;

	virtual const types::Buffer Serialize() const = 0;

	virtual void Deserialize( types::Buffer buffer ) = 0;

	virtual void operator=( const Serializable& other ) {
		// not super efficient, but convenient
		Deserialize( other.Serialize() );
	}

};

}
