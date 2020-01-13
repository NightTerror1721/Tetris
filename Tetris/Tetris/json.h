#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <exception>
#include <filesystem>

using Json = nlohmann::json;

class JsonException : public std::exception
{
public:
	JsonException(const std::string& msg);
};

class JsonSerializable
{
public:
	virtual Json serialize() const = 0;
	virtual void unserialize(const Json& json) = 0;
};

JsonSerializable& operator<< (JsonSerializable& object, const Json& json);
const JsonSerializable& operator>> (const JsonSerializable& object, Json& json);


/* Functions */

namespace json_utils
{

	Json read(const std::filesystem::path& file);
	Json read(const std::string& file);

	void write(const std::filesystem::path& file, const Json& json);
	void write(const std::string& file, const Json& json);


	void inject(JsonSerializable& object, const Json& json);

	Json extract(const JsonSerializable& object);


	void read_and_inject(JsonSerializable& object, const std::filesystem::path& file);
	void read_and_inject(JsonSerializable& object, const std::string& file);

	void extract_and_write(const JsonSerializable& object, const std::filesystem::path& file);
	void extract_and_write(const JsonSerializable& object, const std::string& file);


	inline bool has(const Json& json, const std::string& name)
	{
		return json.find(name) != json.cend();
	}

	template<class _Ty>
	const _Ty& get_or_default(const Json& json, const std::string& name, const _Ty& default_value)
	{
		auto it = json.find(name);
		return it != json.cend() ? it.value().get<_Ty>() : default_value;
	}
}
