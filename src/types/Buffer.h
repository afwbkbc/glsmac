#pragma once

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
	void WriteColor( const Color val );
	const Color ReadColor();
	void WriteData( const void* data, const uint32_t len );
	const void* ReadData( const uint32_t len );

	const std::string ToString() const;

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
