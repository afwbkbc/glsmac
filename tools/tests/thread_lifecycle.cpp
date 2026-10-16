#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <thread>
#include <vector>

#include "lifecycle_services.h"
#include "common/Thread.h"
#include "engine/Engine.h"

// Inject failure at Thread's allocation of its std::thread object. Restrict
// the fault to the caller so concurrent worker allocations remain unaffected.
static thread_local int thread_allocations_before_failure = -1;

void* operator new( std::size_t size ) {
	if ( size == sizeof( std::thread ) && thread_allocations_before_failure >= 0 ) {
		if ( thread_allocations_before_failure-- == 0 ) {
			throw std::bad_alloc();
		}
	}
	if ( auto* result = std::malloc( size ? size : 1 ) ) { return result; }
	throw std::bad_alloc();
}

void operator delete( void* memory ) noexcept { std::free( memory ); }
void operator delete( void* memory, std::size_t ) noexcept { std::free( memory ); }

static void Require( bool condition, const std::string& message ) {
	if ( !condition ) { throw std::runtime_error( message ); }
}

static void CheckThreadFailure( const std::string& phase, bool cleanup_also_fails ) {
	common::Thread thread( "TEST" );
	thread.SetIPS( 100000 );
	LifecycleModule first, second, third;
	std::vector< std::string > events;
	const auto caller = std::this_thread::get_id();
	const auto record = [ & ]( const std::string& event ) {
		Require( std::this_thread::get_id() == caller, "main module left calling thread" );
		events.push_back( event );
	};
	first.on_stop = [ & ]() { record( "first.stop" ); };
	second.on_start = [ & ]() {
		if ( phase == "start" ) { throw std::runtime_error( "primary" ); }
	};
	second.on_iterate = [ & ]() {
		if ( phase == "iterate" ) { throw std::runtime_error( "primary" ); }
	};
	second.on_stop = [ & ]() {
		record( "second.stop" );
		if ( phase == "stop" || cleanup_also_fails ) { throw std::runtime_error( "secondary" ); }
	};
	third.on_stop = [ & ]() {
		record( "third.stop" );
		if ( phase == "stop" ) { throw std::runtime_error( "primary" ); }
	};
	thread.AddModule( &first );
	thread.AddModule( &second );
	thread.AddModule( &third );
	bool caught = false;
	try {
		thread.RunOnCurrentThread( [ & ]() {
			if ( phase == "predicate" ) { throw std::runtime_error( "primary" ); }
			return phase == "stop";
		} );
	}
	catch ( const std::runtime_error& error ) {
		caught = true;
		Require( std::string( error.what() ) == "primary", "cleanup replaced original exception" );
	}
	Require( caught, "injected exception was not propagated" );
	Require( !thread.T_IsRunning(), "failed thread stayed active" );
	Require( first.stops == 1, "first module not stopped exactly once" );
	if ( phase == "start" ) {
		Require( second.stops == 0 && third.starts == 0 && third.stops == 0, "partial startup stopped unstarted module" );
		Require( events == std::vector< std::string >{ "first.stop" }, "wrong partial-start cleanup order" );
	}
	else {
		Require( events == std::vector< std::string >{ "third.stop", "second.stop", "first.stop" }, "cleanup did not continue in reverse order" );
	}
}

struct EngineFixture {
	config::Config config;
	error_handler::ErrorHandler errors;
	logger::Logger logger;
	resource::ResourceManager resources;
	loader::font::FontLoader fonts;
	loader::texture::TextureLoader textures;
	loader::sound::SoundLoader sounds;
	loader::txt::TXTLoaders text;
	scheduler::Scheduler scheduler;
	input::Input input;
	graphics::Graphics graphics;
	audio::Audio audio;
	network::Network network;
	game::backend::Game game;
	std::unique_ptr< engine::Engine > engine;
	unsigned handled = 0;

	explicit EngineFixture( bool single_thread = false ) {
		config.single_thread = single_thread;
		engine = std::make_unique< engine::Engine >(
			&config, &errors, std::vector< logger::Logger* >{ &logger }, &resources,
			&fonts, &textures, &sounds, &text, &scheduler, &input, &graphics, &audio, &network, &game
		);
		errors.on_error = [ this ]( const std::runtime_error& error ) {
			++handled;
			Require( std::string( error.what() ) == "main failure", "wrong engine error reported" );
			CheckStopped();
		};
	}

	void CheckStopped() {
		Require( !network.active && !game.active && !engine->GetGC()->active, "error reported before workers stopped" );
		Require( network.stops == 1 && game.stops == 1 && engine->GetGC()->stops == 1, "worker cleanup incomplete" );
	}
};

static void CheckEngineFailure( const std::string& phase, bool single_thread = false ) {
	EngineFixture fixture( single_thread );
	if ( phase == "start" ) {
		// Workers can still be in Start when the main thread fails.
		fixture.network.on_start = []() { std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ); };
		fixture.graphics.on_start = []() { throw std::runtime_error( "main failure" ); };
	}
	else if ( phase == "iterate" ) {
		fixture.graphics.on_iterate = []() { throw std::runtime_error( "main failure" ); };
	}
	else {
		fixture.input.on_iterate = [ & ]() { fixture.engine->ShutDown(); };
		fixture.graphics.on_stop = []() { throw std::runtime_error( "main failure" ); };
	}
	Require( fixture.engine->Run() == EXIT_FAILURE, "engine did not return failure" );
	Require( fixture.handled == 1, "engine error not handled exactly once" );
	fixture.CheckStopped();
	Require( fixture.input.stops == 1 && fixture.config.stops == 1, "main thread cleanup incomplete" );
}

static void CheckUnexpectedEngineFailure() {
	EngineFixture fixture;
	fixture.graphics.on_iterate = []() { throw std::logic_error( "unexpected" ); };
	bool caught = false;
	try {
		fixture.engine->Run();
	}
	catch ( const std::logic_error& ) {
		caught = true;
	}
	Require( caught, "non-runtime exception was swallowed" );
	fixture.CheckStopped();
}

static void CheckThreadLaunchFailure() {
	common::Thread thread( "FAILED_LAUNCH" );
	thread_allocations_before_failure = 0;
	bool caught = false;
	try {
		thread.T_Start();
	}
	catch ( const std::bad_alloc& ) {
		caught = true;
	}
	thread_allocations_before_failure = -1;
	Require( caught, "thread launch failure was not injected" );
	Require( !thread.T_IsRunning(), "failed launch left thread in starting state" );
}

static void CheckPartialWorkerLaunch() {
	EngineFixture fixture;
	thread_allocations_before_failure = 1;
	bool caught = false;
	try {
		fixture.engine->Run();
	}
	catch ( const std::bad_alloc& ) {
		caught = true;
	}
	thread_allocations_before_failure = -1;
	Require( caught, "second worker launch failure was not injected" );
	Require( fixture.network.starts == 1 && fixture.network.stops == 1 && !fixture.network.active, "launched worker was not stopped" );
	Require( fixture.engine->GetGC()->starts == 0 && fixture.game.starts == 0, "unexpected worker started after launch failure" );
	Require( fixture.config.starts == 0 && fixture.config.stops == 0, "unstarted main modules were touched" );
}

static void CheckNormalEngineShutdown() {
	EngineFixture fixture;
	const auto caller = std::this_thread::get_id();
	fixture.input.on_iterate = [ & ]() {
		Require( std::this_thread::get_id() == caller, "engine main loop left calling thread" );
		fixture.engine->ShutDown();
	};
	Require( fixture.engine->Run() == EXIT_SUCCESS, "normal shutdown failed" );
	Require( fixture.handled == 0, "normal shutdown reported an error" );
	fixture.CheckStopped();
}

int main() {
	try {
		CheckThreadFailure( "start", false );
		CheckThreadFailure( "iterate", false );
		CheckThreadFailure( "iterate", true );
		CheckThreadFailure( "stop", true );
		CheckThreadFailure( "predicate", false );
		CheckEngineFailure( "start" );
		CheckEngineFailure( "iterate" );
		CheckEngineFailure( "stop" );
		CheckEngineFailure( "iterate", true );
		CheckUnexpectedEngineFailure();
		CheckThreadLaunchFailure();
		CheckPartialWorkerLaunch();
		CheckNormalEngineShutdown();
		std::cout << "13 lifecycle regression cases passed\n";
		return EXIT_SUCCESS;
	}
	catch ( const std::exception& error ) {
		std::cerr << "Lifecycle regression: " << error.what() << "\n";
		return EXIT_FAILURE;
	}
}
