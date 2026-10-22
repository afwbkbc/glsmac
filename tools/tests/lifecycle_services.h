#pragma once

// Test doubles for unrelated engine services. Thread.cpp and Engine.cpp are
// compiled unchanged, with these modules replacing graphics, networking, etc.
#include <atomic>
#include <functional>

#include "common/Module.h"

class LifecycleModule : public common::Module {
public:

	const std::string GetClassName() const override { return "LifecycleModule"; }

	void Start() override {
		++starts;
		if ( on_start ) { on_start(); }
		active = true;
	}
	void Iterate() override {
		++iterations;
		if ( on_iterate ) { on_iterate(); }
	}
	void Stop() override {
		++stops;
		active = false;
		if ( on_stop ) { on_stop(); }
	}

	std::atomic< unsigned > starts = 0;
	std::atomic< unsigned > iterations = 0;
	std::atomic< unsigned > stops = 0;
	std::atomic< bool > active = false;
	std::function< void() > on_start;
	std::function< void() > on_iterate;
	std::function< void() > on_stop;
};

namespace config {
class Config : public LifecycleModule {
public:
	enum launch_flag_t { LF_BENCHMARK, LF_SINGLE_THREAD };
	enum debug_flag_t { DF_GSE_ONLY };
	bool single_thread = false;
	bool HasLaunchFlag( launch_flag_t flag ) const { return flag == LF_SINGLE_THREAD && single_thread; }
	bool HasDebugFlag( debug_flag_t ) const { return false; }
	float GetMaxIPS() const { return 100000.0f; }
};
}

namespace error_handler {
class ErrorHandler : public LifecycleModule {
public:
	std::function< void( const std::runtime_error& ) > on_error;
	void HandleError( const std::runtime_error& error ) const { on_error( error ); }
};
}

namespace logger {
class Logger : public LifecycleModule {
public:
	void Log( const std::string& ) const {}
};
}

namespace resource { class ResourceManager : public LifecycleModule {}; }
namespace loader::font { class FontLoader : public LifecycleModule {}; }
namespace loader::texture { class TextureLoader : public LifecycleModule {}; }
namespace loader::sound { class SoundLoader : public LifecycleModule {}; }
namespace loader::txt { class TXTLoaders {}; }
namespace scheduler { class Scheduler : public LifecycleModule {}; }
namespace input { class Input : public LifecycleModule {}; }
namespace graphics { class Graphics : public LifecycleModule {}; }
namespace audio { class Audio : public LifecycleModule {}; }
namespace network { class Network : public LifecycleModule {}; }
namespace game::backend { class Game : public LifecycleModule {}; }
namespace gc {
class GC : public LifecycleModule {
public:
	static constexpr unsigned COLLECTS_PER_SECOND = 100000;
};
}
