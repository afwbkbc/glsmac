#pragma once

#include <mutex>
#include <unordered_set>

#include "common/Module.h"

namespace graphics::opengl {
class OpenGL;
}

namespace gse {
class GC;
}

namespace gc {

class Space;
class Object;

CLASS( GC, common::Module )

	static constexpr uint16_t COLLECTS_PER_SECOND = 1;

	// new objects won't be removed until they are older than this
	// this is important so that temporary variables such as those returned by function are not destroyed in the middle of processing
	// TODO: improve
	static constexpr uint16_t NEW_OBJECT_PROTECTION_TIME_MS = 200;

	void Start() override;
	void Stop() override;
	void Iterate() override;

#if defined( DEBUG ) || defined( FASTDEBUG )
	static const bool IsDebugEnabled();
	static void DebugLock();
	static void DebugBegin( const std::string& what );
	static void DebugEntry( const std::string& what, gc::Object* const object );
	static void DebugEnd();
	static void DebugUnlock();
#endif

private:
	std::mutex m_spaces_mutex;
	std::unordered_set< Space* > m_spaces = {};

private:
	friend class Space;
	void AddSpace( Space* const space );
	void RemoveSpace( Space* const space );

};

}
