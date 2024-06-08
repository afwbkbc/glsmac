#pragma once

#include <string>

#include "types/Serializable.h"
#include "gse/Wrappable.h"

#include "types/Color.h"

namespace game {
namespace rules {

CLASS2( Faction, types::Serializable, gse::Wrappable )

	Faction();
	Faction( const std::string& id, const std::string& name );

	typedef uint8_t faction_flag_t;
	static const faction_flag_t FF_NONE;
	static const faction_flag_t FF_PROGENITOR;

	std::string m_id = "";
	std::string m_name = "";
	faction_flag_t m_flags = FF_NONE;

	struct {
		types::Color text = {};
		types::Color border = {};
	} m_colors = {};

	struct {
		std::string file = "";
		size_t grid_x = 0;
		size_t grid_y = 0;
		size_t cell_width = 0;
		size_t cell_height = 0;
		size_t cell_cx = 0;
		size_t cell_cy = 0;
		size_t cell_padding = 0;
		float scale_x = 1.0f;
		float scale_y = 1.0f;
	} m_bases_render = {};

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

	WRAPDEFS_PTR( Faction )

};

}
}
