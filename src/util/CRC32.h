#pragma once

#include "Util.h"
#include "types/Buffer.h"

namespace util {

CLASS( CRC32, Util )

	typedef uint32_t crc_t;

	static const crc_t CalculateFromBuffer( const types::Buffer& buf );

};

}
