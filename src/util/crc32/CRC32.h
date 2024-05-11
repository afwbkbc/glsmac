#pragma once

#include "util/Util.h"

#include "types/Buffer.h"
#include "Types.h"

namespace util {
namespace crc32 {

CLASS( CRC32, Util )

	static const crc_t CalculateFromBuffer( const types::Buffer& buf );

};

}
}
