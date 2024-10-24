#pragma once

#include <string>
#include <vector>

#include "Types.h"
#include "types/Buffer.h"

namespace game {
namespace backend {
namespace base {

class PopDef {
public:

	typedef uint8_t pop_flags_t;
	static constexpr pop_flags_t PF_NONE = 0;
	static constexpr pop_flags_t PF_TILE_WORKER = 1 << 0;

	PopDef(
		const std::string& id,
		const std::string& name,
		const pop_render_infos_t& renders_human,
		const pop_render_infos_t& renders_progenitor,
		const pop_flags_t flags
	);
	virtual ~PopDef() = default;

	const std::string m_id;
	const std::string m_name;
	const pop_render_infos_t m_renders_human;
	const pop_render_infos_t m_renders_progenitor;
	const pop_flags_t m_flags;

	const std::string ToString( const std::string& prefix = "" ) const;

	static const types::Buffer Serialize( const PopDef* def );
	static PopDef* Unserialize( types::Buffer& buf );

private:
	const std::string InfosToString( const std::string& prefix, const std::string& name, const pop_render_infos_t& infos ) const;

};

}
}
}
