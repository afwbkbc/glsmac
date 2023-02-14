#pragma once

#include "base/Base.h"

#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"

namespace types {
	
CLASS( Buffer, base::Base )
	
	static constexpr size_t BUFFER_ALLOC_CHUNK = 1024;
	
	typedef uint8_t data_t;
	typedef char checksum_t;
	
	Buffer();
	Buffer( const std::string& strval );
	~Buffer();
	
	Buffer( Buffer& other);
	
	data_t* data;
	data_t* dw;
	data_t* dr;
	size_t allocated_len;
	size_t lenw;
	size_t lenr;
	checksum_t checksum;
	
	void WriteBool( const bool val );
	const bool ReadBool();
	void WriteInt( const long long int val );
	const long long int ReadInt();
	void WriteFloat( const float val );
	const float ReadFloat();
	void WriteString( const std::string& val );
	const std::string ReadString();
	void WriteVec2f( const Vec2< float > val );
	const Vec2< float > ReadVec2f();
	void WriteVec3( const Vec3 val );
	const Vec3 ReadVec3();
	void WriteColor( const Color val );
	const Color ReadColor();
	void WriteData( const void* data, const size_t len );
	const void* ReadData( const size_t len );
	
	const std::string ToString() const;
	
private:
	
	enum type_t : uint8_t {
		
		T_NONE,
		T_BOOL,
		T_INT,
		T_FLOAT,
		T_STRING,
		T_VEC2F,
		T_VEC3,
		T_COLOR,
		T_DATA,
		
		T_MAX
	};
	
	void WriteImpl( const type_t type, const char* s, const size_t sz );
	char* ReadImpl( const type_t need_type, char* s, size_t* sz, const size_t need_sz = 0 );
	void Alloc( size_t size );
	
};

}
