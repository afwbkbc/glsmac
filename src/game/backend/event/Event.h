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

	Event( Game* game, const size_t caller, GSE_CALLABLE, const std::string& name, const gse::value::object_properties_t& data );

	const types::Buffer Serialize() const;
	void Unserialize( GSE_CALLABLE, types::Buffer buffer );

	const std::string ToString() const;

	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

	const size_t GetCaller() const;
	const std::string& GetName() const;
	const gse::value::object_properties_t& GetData() const;

private:
	Game* m_game = nullptr;
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
