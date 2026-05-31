#pragma once

#include <string>
#include <cstdint>

#include "types/Buffer.h"
#include "Types.h"

namespace game {
namespace backend {
namespace resource {

class ResourceManager;

class Resource {
public:

	Resource( const std::string& id, const std::string& name, const render_info_t& render_info );

	const std::string m_id;
	const std::string m_name;
	const render_info_t m_render_info;

	const std::string ToString( const std::string& prefix = "" ) const;

	static const types::Buffer Serialize( const Resource* resource );
	static Resource* Deserialize( types::Buffer& buf );

};

}
}
}
