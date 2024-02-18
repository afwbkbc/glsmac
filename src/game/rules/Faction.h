#pragma once

#include <string>

#include "types/Serializable.h"

#include "types/Color.h"

#include "gse/Value.h"

namespace game {
namespace rules {

CLASS( Faction, types::Serializable )

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

	void ImportPCX( const std::string& pcx_file );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

	WRAPDEFS_PTR( Faction )

};

}
}
