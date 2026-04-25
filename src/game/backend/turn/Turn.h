#pragma once

#include <vector>

#include "util/crc32/CRC32.h"

namespace game {
namespace backend {
namespace turn {

class Turn {
public:

	Turn();
	~Turn();

	const size_t GetId() const;

	void AdvanceTurn( const size_t turn_id );
	const util::crc32::crc_t FinalizeAndChecksum();

	void Reset();

private:
	size_t m_id = 0;
};

}
}
}
