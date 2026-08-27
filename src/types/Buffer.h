#pragma once

#include <limits>
#include <type_traits>

#include "common/Common.h"

#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"

namespace types {

CLASS( Buffer, common::Class )

	static constexpr uint32_t BUFFER_ALLOC_CHUNK = 1024;

	typedef uint8_t data_t;
	typedef uint8_t checksum_t;

	Buffer();
	Buffer( const std::string& strval );
	~Buffer();

	Buffer( const Buffer& other );
	Buffer( Buffer&& other ) noexcept;
	Buffer& operator=( const Buffer& other );
	Buffer& operator=( Buffer&& other ) noexcept;

	data_t* data;
	data_t* dw;
	data_t* dr;
	uint32_t allocated_len;
	uint32_t lenw;
	uint32_t lenr;

	void WriteBool( const bool val );
	const bool ReadBool();
	void WriteInt( const long long int val );
	const long long int ReadInt();
	template< typename T >
	const T ReadInt( const std::string& name ) {
		static_assert( std::is_integral< T >::value && !std::is_same< T, bool >::value, "integer type required" );
		const auto value = ReadInt();
		bool is_invalid = false;
		if constexpr ( std::is_signed< T >::value ) {
			is_invalid = value < static_cast< long long int >( ( std::numeric_limits< T >::min )() ) ||
				value > static_cast< long long int >( ( std::numeric_limits< T >::max )() );
		}
		else {
			is_invalid = value < 0 ||
				static_cast< unsigned long long int >( value ) > static_cast< unsigned long long int >( ( std::numeric_limits< T >::max )() );
		}
		if ( is_invalid ) {
			THROW( "invalid serialized " + name + ": " + std::to_string( value ) );
		}
		return static_cast< T >( value );
	}
	const size_t ReadCollectionSize( const std::string& name );
	void WriteFloat( const float val );
	const float ReadFloat();
	void WriteString( const std::string& val );
	const std::string ReadString();
	void WriteVec2u( const Vec2< uint32_t > val );
	const Vec2< uint32_t > ReadVec2u();
	void WriteVec2f( const Vec2< float > val );
	const Vec2< float > ReadVec2f();
	void WriteVec3( const types::Vec3 val );
	const types::Vec3 ReadVec3();
	void WriteColor( const Color& val );
	void ReadColor( Color& val );
	void WriteData( const void* data, const uint32_t len );
	const void* ReadData( const uint32_t len );

	const std::string ToString() const;
	const uint32_t GetRemaining() const;

private:

	enum type_t : uint8_t {

		T_NONE,
		T_BOOL,
		T_INT,
		T_FLOAT,
		T_STRING,
		T_VEC2U,
		T_VEC2F,
		T_VEC3,
		T_COLOR,
		T_DATA,

		T_MAX
	};

	void WriteImpl( const type_t type, const char* s, const uint32_t sz );
	char* ReadImpl( const type_t need_type, char* s, uint32_t* sz, const uint32_t need_sz = 0 );
	void Alloc( uint32_t size );

};

}
