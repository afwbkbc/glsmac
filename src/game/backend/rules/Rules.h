#pragma once

#include <unordered_map>
#include <vector>

#include "types/Serializable.h"

#include "DifficultyLevel.h"

namespace game {
namespace backend {
namespace rules {

CLASS( Rules, types::Serializable )

	std::map< size_t, DifficultyLevel > m_difficulty_levels;

	virtual const DifficultyLevel& GetDefaultDifficultyLevel() const = 0;

	void Initialize();

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

protected:
	virtual void InitRules() = 0;

private:
	bool m_is_initialized = false;

};

}
}
}
