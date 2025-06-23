#pragma once

#include "gc/Object.h"
#include "gse/value/Types.h"
#include "gse/Value.h"
#include "types/Buffer.h"

namespace game {
namespace backend {

class Game;

namespace event {

class Event : public gc::Object {
public:

	enum source_t {
		ES_LOCAL,
		ES_SERVER,
		ES_CLIENT,
	};

	Event( Game* game, const source_t source, const size_t caller, GSE_CALLABLE, const std::string& name, const gse::value::object_properties_t& data, const std::string& id = "" );

	const types::Buffer Serialize() const;
	static Event* const Unserialize( Game* const game, const source_t source, GSE_CALLABLE, types::Buffer buffer );

	const std::string ToString() const;

	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

	const source_t GetSource() const;
	const size_t GetCaller() const;
	const std::string& GetId() const;
	const std::string& GetEventName() const;
	const gse::value::object_properties_t& GetData() const;

private:
	Game* const m_game = nullptr;
	const source_t m_source = ES_LOCAL;
	const size_t m_caller = 0;
	std::string m_id = 0;
	std::string m_name = "";
	gse::value::object_properties_t m_original_data = {};
	gse::value::object_properties_t m_data = {};
	gse::value::function_arguments_t m_args = {};

	void UpdateData( GSE_CALLABLE );

};

}
}
}
