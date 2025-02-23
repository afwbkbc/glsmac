#pragma once

#include <mutex>
#include <unordered_set>

#include "common/Module.h"

namespace gse {
class GC;
}

namespace gc {

class Space;

CLASS( GC, common::Module )

	static constexpr uint16_t COLLECTS_PER_SECOND = 1;
	static constexpr uint16_t NEW_OBJECT_PROTECTION_TIME_MS = 1500; // new objects won't be removed until they are older than this

	void Start() override;
	void Stop() override;
	void Iterate() override;

private:
	std::mutex m_spaces_mutex;
	std::unordered_set< Space* > m_spaces = {};

private:
	friend class Space;
	void AddSpace( Space* const space );
	void RemoveSpace( Space* const space );

};

}
