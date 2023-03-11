#pragma once

#include "Util.h"

namespace util {

template <typename DATATYPE>
CLASS( Clamper, Util )

	struct range_t {
		DATATYPE min = 0;
		DATATYPE max = 0;
		bool operator!= ( const range_t& other ) const {
			return (
				min != other.min ||
				max != other.max
			);
		};
	};
	struct ranges_t {
		range_t src = {};
		range_t dst = {};
		bool operator!= ( const ranges_t& other ) const {
			return (
				src != other.src ||
				dst != other.dst
			);
		};
	};

	Clamper() {}
	Clamper( const ranges_t& ranges ) {
		SetRange( ranges );
	}
	
	void SetSrcRange( const range_t& range ) {
		m_ranges.src = range;
	}
	const range_t& GetSrcRange() {
		return m_ranges.src;
	}
	void SetDstRange( const range_t& range ) {
		m_ranges.dst = range;
	}
	const range_t& GetDstRange() {
		return m_ranges.dst;
	}
	void SetRange( const ranges_t& ranges ) {
		m_ranges = ranges;
	}
	const ranges_t& GetRange() {
		return m_ranges;
	}
	
	 // true if was change, false if it's already same
	bool SetSrcRangeMaybe( const range_t& range ) {
		if ( m_ranges.src != range ) {
			m_ranges.src = range;
			return true;
		}
		return false;
	}
	bool SetDstRangeMaybe( const range_t& range ) {
		if ( m_ranges.dst != range ) {
			m_ranges.dst = range;
			return true;
		}
		return false;
	}
	bool SetRangeMaybe( const ranges_t& ranges ) {
		if ( m_ranges != ranges ) {
			m_ranges = ranges;
			return true;
		}
		return false;
	}
	
	DATATYPE Clamp( const DATATYPE value ) const {
		//Log( "Clamping " + std::to_string( value ) + " from [" + std::to_string( m_ranges.src.min ) + " " + std::to_string( m_ranges.src.max ) + "] to [" + std::to_string( m_ranges.dst.min ) + " " + std::to_string( m_ranges.dst.max ) + "]" );
		float result = (float) ( value - m_ranges.src.min ) * (float) ( (float) ( m_ranges.dst.max - m_ranges.dst.min ) / ( m_ranges.src.max - m_ranges.src.min ) );
		if ( m_inversed )
			result = m_ranges.dst.max - result;
		else
			result += m_ranges.dst.min;
		if ( !m_overflow_allowed ) {
			if ( result < m_ranges.dst.min ) {
				result = m_ranges.dst.min;
			}
			if ( result > m_ranges.dst.max ) {
				result = m_ranges.dst.max;
			}
		}
		//Log( "RESULT = " + std::to_string( result ) );
		return (DATATYPE) result;
	}
	DATATYPE Unclamp( const DATATYPE value ) const {
		//Log( "Unclamping " + std::to_string( value ) + " from [" + std::to_string( m_ranges.src.min ) + " " + std::to_string( m_ranges.src.max ) + "] to [" + std::to_string( m_ranges.dst.min ) + " " + std::to_string( m_ranges.dst.max ) + "]" );
		float result = (float) ( value - m_ranges.dst.min ) * (float) ( (float) ( m_ranges.src.max - m_ranges.src.min ) / ( m_ranges.dst.max - m_ranges.dst.min ) );
		if ( m_inversed )
			result = m_ranges.src.max - result;
		else
			result += m_ranges.src.min;
		if ( !m_overflow_allowed ) {
			if ( result < m_ranges.src.min ) {
				result = m_ranges.src.min;
			}
			if ( result > m_ranges.src.max ) {
				result = m_ranges.src.max;
			}
		}
		//Log( "RESULT = " + std::to_string( result ) );
		return (DATATYPE) result;
	}
	void SetInversed( const bool inversed ) {
		m_inversed = true;
	}
	void SetOverflowAllowed( const bool overflow_allowed ) {
		m_overflow_allowed = overflow_allowed;
	}
	
	void operator= ( const Clamper& other ) {
		m_ranges = other.m_ranges;
		m_inversed = other.m_inversed;
	}
	
protected:
	ranges_t m_ranges = {};
	bool m_inversed = false;
	bool m_overflow_allowed = false;
};

} /* namespace util */
