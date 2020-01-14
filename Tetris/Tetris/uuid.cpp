#include "uuid.h"

#include <chrono>

using namespace std::chrono;

typedef union
{
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
	if (!(last + 1))
		++last;

	code_t code;
	code.value = ++last;
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
std::ostream& operator<< (std::ostream& os, const UUID& uuid) { return os << to_string(uuid._code); }
