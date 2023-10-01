#pragma once

#include <string>

#include "types/Serializable.h"

namespace game {
namespace rules {

CLASS( DifficultyLevel, types::Serializable )

	DifficultyLevel();
	DifficultyLevel( const std::string& name, const int difficulty );

	std::string m_name = "";
	int m_difficulty = 0;

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

}
}
