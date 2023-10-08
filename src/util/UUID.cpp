#include "UUID.h"

#include <uuid++.hh>

namespace util {

const std::string UUID::GenerateRandom() {
	uuid id;
	id.make( UUID_MAKE_V1 );
	const char* myId = id.string();
	std::string result( id.string() );
	delete myId;
	return result;
}

}
