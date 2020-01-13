#pragma once

#include <cinttypes>
#include <string>
#include <iostream>

class UUID
{
private:
	uint64_t _code;

public:
	UUID(const UUID& uuid);

	UUID& operator= (const UUID& uuid);

	bool operator== (const UUID& uuid) const;
	bool operator!= (const UUID& uuid) const;
	bool operator> (const UUID& uuid) const;
	bool operator< (const UUID& uuid) const;
	bool operator>= (const UUID& uuid) const;
	bool operator<= (const UUID& uuid) const;

	operator bool() const;
	bool operator! () const;

private:
	UUID(const uint64_t code);

public:
	static UUID generate();
	static const UUID Invalid;

	friend std::string to_string(const UUID& uuid);
	friend std::istream& operator>> (std::istream& is, UUID& uuid);
	friend std::ostream& operator<< (std::ostream& os, const UUID& uuid);
};

std::string to_string(const UUID& uuid);

std::istream& operator>> (std::istream& is, UUID& uuid);
std::ostream& operator<< (std::ostream& os, const UUID& uuid);
