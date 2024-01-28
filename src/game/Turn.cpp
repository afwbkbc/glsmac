#include "Turn.h"

namespace game {

Turn::Turn( const size_t year )
	: m_year( year ) {

}

const size_t Turn::GetYear() const {
	return m_year;
}

}