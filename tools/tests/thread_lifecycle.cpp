#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <thread>
#include <vector>

#include "lifecycle_services.h"
#include "common/Thread.h"
#include "engine/Engine.h"

// Include Thread's std::thread allocation, but do not assume every allocation
// of this size starts another worker: library launch bookkeeping may match too.
// Restrict faults to the caller so worker allocations remain unaffected.
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

static void CheckThreadFailure( const std::string& phase, bool cleanup_also_fails, bool hook_also_fails = false ) {
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
			return phase == "stop" || phase == "before_stop";
		}, [ & ]() {
			record( "before_stop" );
			if ( phase == "before_stop" ) { throw std::runtime_error( "primary" ); }
			if ( hook_also_fails ) { throw std::runtime_error( "hook failure" ); }
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
		Require( events == std::vector< std::string >{ "before_stop", "first.stop" }, "wrong partial-start cleanup order" );
	}
	else {
		Require( events == std::vector< std::string >{ "before_stop", "third.stop", "second.stop", "first.stop" }, "cleanup did not continue in reverse order" );
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
	std::atomic< unsigned > worker_final_logs = 0;
	bool early_main_cleanup = false;

	explicit EngineFixture( bool single_thread = false ) {
		config.single_thread = single_thread;
		engine = std::make_unique< engine::Engine >(
			&config, &errors, std::vector< logger::Logger* >{ &logger }, &resources,
			&fonts, &textures, &sounds, &text, &scheduler, &input, &graphics, &audio, &network, &game
		);
		if ( !single_thread ) {
			engine->AddLogCallback( this, [ this ]( const std::string& text ) {
				if ( text.find( "Thread stopped" ) != std::string::npos &&
					( text.find( "NETWORK" ) != std::string::npos || text.find( "GC" ) != std::string::npos || text.find( "GAME" ) != std::string::npos ) ) {
					++worker_final_logs;
				}
			} );
			LifecycleModule* main_modules[] = { &config, &errors, &fonts, &textures, &sounds, &logger, &resources, &input, &graphics, &audio, &scheduler };
			for ( auto* module : main_modules ) {
				module->on_stop = [ this ]() { CheckMainCleanupOrder(); };
			}
		}
		errors.on_error = [ this ]( const std::runtime_error& error ) {
			++handled;
			Require( std::string( error.what() ) == "main failure", "wrong engine error reported" );
			CheckStopped();
		};
	}

	void CheckMainCleanupOrder() {
		// Record failures so an earlier injected exception cannot mask them.
		early_main_cleanup = early_main_cleanup || network.active || game.active || engine->GetGC()->active || worker_final_logs != 3;
	}

	void CheckStopped() {
		Require( !early_main_cleanup, "main module cleanup began before workers finished" );
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
		fixture.graphics.on_stop = [ & ]() {
			if ( !single_thread ) { fixture.CheckMainCleanupOrder(); }
			throw std::runtime_error( "main failure" );
		};
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

static void CheckWorkerStopCommandFailure( bool primary_failure ) {
	EngineFixture fixture;
	auto caller = std::this_thread::get_id();
	fixture.engine->AddLogCallback( &caller, [ & ]( const std::string& text ) {
		if ( std::this_thread::get_id() == caller && text.find( "NETWORK" ) != std::string::npos && text.find( "Sent STOP command" ) != std::string::npos ) {
			throw std::runtime_error( primary_failure ? "secondary failure" : "main failure" );
		}
	} );
	fixture.input.on_iterate = [ & ]() {
		if ( primary_failure ) { throw std::runtime_error( "main failure" ); }
		fixture.engine->ShutDown();
	};
	Require( fixture.engine->Run() == EXIT_FAILURE, "worker stop command failure was not reported" );
	Require( fixture.handled == 1, "worker stop command failure was not handled exactly once" );
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
	bool partial_launch_exercised = false;
	bool launch_succeeded = false;
	unsigned failure_positions = 0;
	// Discover allocation positions rather than encoding a standard library's
	// allocation count. The bound prevents an unexpectedly long test sweep.
	for ( int position = 0; position < 64; ++position ) {
		EngineFixture fixture;
		// Stop injecting once worker launch is complete. A position beyond the
		// launch allocations must still terminate through normal main shutdown.
		fixture.config.on_start = []() { thread_allocations_before_failure = -1; };
		fixture.input.on_iterate = [ & ]() { fixture.engine->ShutDown(); };
		thread_allocations_before_failure = position;
		bool caught = false;
		int result = EXIT_FAILURE;
		try {
			result = fixture.engine->Run();
		}
		catch ( const std::bad_alloc& ) {
			caught = true;
		}
		thread_allocations_before_failure = -1;
		LifecycleModule* workers[] = { &fixture.network, fixture.engine->GetGC(), &fixture.game };
		unsigned started = 0;
		bool unstarted_seen = false;
		for ( const auto* worker : workers ) {
			const auto starts = worker->starts.load();
			Require( starts <= 1 && worker->stops == starts && !worker->active, "launched worker was not stopped exactly once" );
			Require( !unstarted_seen || starts == 0, "worker launch did not stop at the failure" );
			unstarted_seen = unstarted_seen || starts == 0;
			started += starts;
		}
		Require( fixture.handled == 0, "allocation failure reached the runtime error handler" );
		if ( caught ) {
			++failure_positions;
			partial_launch_exercised = partial_launch_exercised || ( started > 0 && started < 3 );
			Require( fixture.config.starts == 0 && fixture.config.stops == 0, "unstarted main modules were touched" );
		}
		else {
			Require( result == EXIT_SUCCESS && started == 3, "allocation sweep did not finish normal startup" );
			fixture.CheckStopped();
			Require( fixture.config.starts == 1 && fixture.config.stops == 1, "normal main shutdown incomplete" );
			launch_succeeded = true;
			break;
		}
	}
	Require( partial_launch_exercised, "allocation sweep never exercised a partial worker launch" );
	Require( launch_succeeded, "worker launch allocation sweep exceeded its bound" );
	std::cout << "Worker launch sweep covered " << failure_positions << " allocation failure positions\n";
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

static void CheckWorkerJoinAfterCleanup() {
	EngineFixture fixture;
	common::Thread worker( "JOIN_BOUNDARY" );
	worker.SetIPS( 100000 );
	LifecycleModule module;
	worker.AddModule( &module );
	std::promise< void > final_log_entered, release_final_log;
	auto entered = final_log_entered.get_future();
	auto release = release_final_log.get_future();
	fixture.engine->AddLogCallback( &worker, [ & ]( const std::string& text ) {
		if ( text.find( "JOIN_BOUNDARY" ) != std::string::npos && text.find( "Thread stopped" ) != std::string::npos ) {
			final_log_entered.set_value();
			release.wait();
		}
	} );
	worker.T_Start();
	worker.T_Stop();
	entered.wait();
	// Module cleanup has completed, but the worker still uses the engine.
	// T_IsRunning must not report completion until that final use returns.
	auto running = std::async( std::launch::async, [ & ]() { return worker.T_IsRunning(); } );
	const auto status = running.wait_for( std::chrono::milliseconds( 20 ) );
	release_final_log.set_value();
	const auto was_running = running.get();
	fixture.engine->RemoveLogCallback( &worker );
	Require( status == std::future_status::timeout && !was_running, "worker was not joined after module cleanup" );
	Require( module.stops == 1 && !module.active, "worker cleanup incomplete" );
}

int main() {
	try {
		CheckThreadFailure( "start", false, true );
		CheckThreadFailure( "iterate", false );
		CheckThreadFailure( "iterate", true, true );
		CheckThreadFailure( "stop", true );
		CheckThreadFailure( "predicate", false );
		CheckThreadFailure( "before_stop", true );
		CheckEngineFailure( "start" );
		CheckEngineFailure( "iterate" );
		CheckEngineFailure( "stop" );
		CheckEngineFailure( "iterate", true );
		CheckUnexpectedEngineFailure();
		CheckWorkerStopCommandFailure( false );
		CheckWorkerStopCommandFailure( true );
		CheckThreadLaunchFailure();
		CheckPartialWorkerLaunch();
		CheckNormalEngineShutdown();
		CheckWorkerJoinAfterCleanup();
		std::cout << "17 lifecycle regression cases passed\n";
		return EXIT_SUCCESS;
	}
	catch ( const std::exception& error ) {
		std::cerr << "Lifecycle regression: " << error.what() << "\n";
		return EXIT_FAILURE;
	}
}
