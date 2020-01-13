#include "json.h"

#include <fstream>
#include <iostream>

namespace fs = std::filesystem;


JsonException::JsonException(const std::string& msg) :
	exception{ msg.c_str() }
{}


JsonSerializable& operator<< (JsonSerializable& object, const Json& json)
{
	object.unserialize(json);
	return object;
}
const JsonSerializable& operator>> (const JsonSerializable& object, Json& json)
{
	json = std::move(object.serialize());
	return object;
}


/* Functions */

Json json_utils::read(const std::filesystem::path& file)
{
	try
	{
		std::ifstream f{ file, std::ios::in };
		Json json;
		f >> json;
		f.close();
		return std::move(json);
	}
	catch (...)
	{
		std::cerr << "An error has been ocurred during json reading in file: " << file << std::endl;
		throw JsonException{ "Read error in method: json_utils::read" };
	}
}
Json json_utils::read(const std::string& file)
{
	return std::move(json_utils::read(fs::path{ file }));
}

void json_utils::write(const std::filesystem::path& file, const Json& json)
{
	try
	{
		std::ofstream f{ file, std::ios::out };
		f << json;
		f.close();
	}
	catch (...)
	{
		std::cerr << "An error has been ocurred during json writting in file: " << file << std::endl;
		throw JsonException{ "Read error in method: json_utils::write" };
	}
}
void json_utils::write(const std::string& file, const Json& json)
{
	json_utils::write(fs::path{ file }, json);
}


void json_utils::inject(JsonSerializable& object, const Json& json)
{
	object.unserialize(json);
}

Json json_utils::extract(const JsonSerializable& object)
{
	return std::move(object.serialize());
}


void json_utils::read_and_inject(JsonSerializable& object, const std::filesystem::path& file)
{
	json_utils::inject(object, json_utils::read(file));
}
void json_utils::read_and_inject(JsonSerializable& object, const std::string& file)
{
	json_utils::inject(object, json_utils::read(file));
}

void json_utils::extract_and_write(const JsonSerializable& object, const std::filesystem::path& file)
{
	json_utils::write(file, json_utils::extract(object));
}
void json_utils::extract_and_write(const JsonSerializable& object, const std::string& file)
{
	json_utils::write(file, json_utils::extract(object));
}
