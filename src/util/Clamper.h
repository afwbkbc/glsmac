#pragma once

#include "Util.h"

namespace util {

template <typename DATATYPE>
CLASS( Clamper, Util )
	Clamper() {}
	Clamper( const DATATYPE src_min, const DATATYPE src_max, const DATATYPE dst_min, const DATATYPE dst_max ) {
		SetRange( src_min, src_max, dst_min, dst_max );
	}
	void SetSrcRange( const DATATYPE src_min, const DATATYPE src_max ) {
		m_src_min = src_min;
		m_src_max = src_max;
	}
	void SetDstRange( const DATATYPE dst_min, const DATATYPE dst_max ) {
		m_dst_min = dst_min;
		m_dst_max = dst_max;
	}
	void SetRange( const DATATYPE src_min, const DATATYPE src_max, const DATATYPE dst_min, const DATATYPE dst_max ) {
		SetSrcRange( src_min, src_max );
		SetDstRange( dst_min, dst_max );
	}
	DATATYPE Clamp( const DATATYPE value ) const {
		//Log( "Clamping " + to_string( value ) + " from [" + to_string( m_src_min ) + " " + to_string( m_src_max ) + "] to [" + to_string( m_dst_min ) + " " + to_string( m_dst_max ) + "]" );
		float result = (float) ( value - m_src_min ) * (float) ( (float) ( m_dst_max - m_dst_min ) / ( m_src_max - m_src_min ) );
		if ( m_inversed )
			result = m_dst_max - result;
		else
			result += m_dst_min;
		if ( !m_overflow_allowed ) {
			if ( result < m_dst_min ) {
				result = m_dst_min;
			}
			if ( result > m_dst_max ) {
				result = m_dst_max;
			}
		}
		//Log( "RESULT = " + to_string( result ) );
		return (DATATYPE) result;
	}
	void SetInversed( const bool inversed ) {
		m_inversed = true;
	}
	void SetOverflowAllowed( const bool overflow_allowed ) {
		m_overflow_allowed = overflow_allowed;
	}
	
	void operator= ( const Clamper& other ) {
		m_src_min = other.m_src_min;
		m_src_max = other.m_src_max;
		m_dst_min = other.m_dst_min;
		m_dst_max = other.m_dst_max;
		m_inversed = other.m_inversed;
	}
	
protected:
	float m_src_min = 0;
	float m_src_max = 0;
	float m_dst_min = 0;
	float m_dst_max = 0;
	bool m_inversed = false;
	bool m_overflow_allowed = false;
};

} /* namespace util */
