#include "uuid.h"

#include <chrono>
#include <random>

using namespace std::chrono;

typedef union
{
	struct
	{
		uint32_t time;
		uint32_t rand;
	};
	char bytes[8];
	uint64_t value;
} code_t;

UUID::UUID(const uint64_t code) :
	_code{ code }
{}
UUID::UUID(const UUID& uuid) :
	_code{ uuid._code }
{}

UUID& UUID::operator= (const UUID& uuid)
{
	_code = uuid._code;
	return *this;
}

bool UUID::operator== (const UUID& uuid) const { return _code == uuid._code; }
bool UUID::operator!= (const UUID& uuid) const { return _code != uuid._code; }
bool UUID::operator> (const UUID& uuid) const { return _code > uuid._code; }
bool UUID::operator< (const UUID& uuid) const { return _code < uuid._code; }
bool UUID::operator>= (const UUID& uuid) const { return _code >= uuid._code; }
bool UUID::operator<= (const UUID& uuid) const { return _code <= uuid._code; }

UUID::operator bool() const { return _code; }
bool UUID::operator! () const { return !_code; }

UUID UUID::generate()
{
	static uint64_t last = system_clock::now().time_since_epoch().count();

	uint64_t tt = system_clock::now().time_since_epoch().count();
	std::mt19937 gen{ static_cast<uint32_t>((tt * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)) };
	last = tt;

	code_t code;
	code.time = (tt >> 10U) & 0xffffffffU;
	code.rand = gen();
	return { code.value };
}

const UUID UUID::Invalid{ 0 };


std::string to_string(const UUID& uuid)
{
	static char VALUES[] = "0123456789abcdef";

	code_t code;
	code.value = uuid._code;

	std::string str{ "0000000000000000" };
	for (char* s_ptr = str.data(), *c_ptr = code.bytes; *s_ptr != '\0'; ++s_ptr, ++c_ptr)
	{
		*s_ptr = VALUES[(*c_ptr >> 4U) & 0xfU];
		*(++s_ptr) = VALUES[*c_ptr & 0xfU];
	}
	return str;
}

std::istream& operator>> (std::istream& is, UUID& uuid) { return is >> uuid._code; }
std::ostream& operator<< (std::ostream& os, const UUID& uuid) { return os << uuid._code; }
