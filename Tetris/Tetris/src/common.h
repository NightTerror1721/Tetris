#pragma once

#include <type_traits>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <compare>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <new>

#ifndef __cpp_lib_concepts
#define __cpp_lib_concepts
#endif
#include <concepts>

#include "nlohmann/json.hpp"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"


typedef std::uint8_t UInt8;
typedef std::uint16_t UInt16;
typedef std::uint32_t UInt32;
typedef std::uint64_t UInt64;

typedef std::int8_t Int8;
typedef std::int16_t Int16;
typedef std::int32_t Int32;
typedef std::int64_t Int64;

typedef std::string String;

typedef std::byte Byte;

typedef std::size_t Size;
typedef std::size_t Offset;

typedef std::filesystem::path Path;

typedef sf::Vector2f Vec2f;
typedef sf::Vector2i Vec2i;
typedef sf::Vector2u Vec2u;

using Json = nlohmann::json;

template<typename _Fty>
using Function = std::function<_Fty>;

namespace filesystem = std::filesystem;

namespace utils
{
	template<typename _Ty = Byte>
	inline _Ty* malloc_raw(Size size) { return reinterpret_cast<_Ty*>(::operator new(size)); }

	inline void free_raw(void* ptr) { ::operator delete(ptr); }


	template<typename _Ty, typename... _Args>
	inline _Ty& construct(_Ty& object, _Args&&... args) { return new (&object) _Ty{ std::forward<_Args>(args)... }, object; }

	template<typename _Ty>
	inline void destroy(_Ty& object) { object.~_Ty(); }

	template<typename _Ty>
	inline _Ty& copy(_Ty& dst, const _Ty& src) { return construct<_Ty, const _Ty&>(dst, src); }

	template<typename _Ty>
	inline _Ty& move(_Ty& dst, _Ty&& src) { return construct<_Ty, _Ty&&>(dst, std::move(src)); }


	template<typename _Base, typename _Derived>
	concept BaseOf = std::is_base_of<_Base, _Derived>::value;


	template<unsigned int _BitIdx, unsigned int _BitCount, typename _Ty = UInt8>
	constexpr _Ty get_bits(_Ty value)
	{
		return static_cast<_Ty>((value >> _BitIdx) & ((0x1 << _BitCount) - 1));
	}

	template<unsigned int _BitIdx, unsigned int _BitCount, typename _Ty = UInt8>
	constexpr _Ty set_bits(_Ty base, _Ty bits)
	{
		_Ty mask = ((0x1 << _BitCount) - 1);
		_Ty value = ((bits & mask) << _BitIdx) & ~(_Ty(0));
		return (base & ~(mask << _BitIdx)) | value;
	}

	template<typename _ValueTy, typename _MinTy, typename _MaxTy>
	constexpr _ValueTy clamp(_ValueTy value, _MinTy min, _MaxTy max)
	{
		if constexpr (std::same_as< _MinTy, _ValueTy>)
		{
			if constexpr (std::same_as< _MaxTy, _ValueTy>)
				return std::max(std::min(max, value), min);
			else return std::max(std::min(static_cast<_ValueTy>(max), value), min);
		}
		else
		{
			if constexpr (std::same_as< _MaxTy, _ValueTy>)
				return std::max(std::min(max, value), static_cast<_ValueTy>(min));
			else return std::max(std::min(static_cast<_ValueTy>(max), value), static_cast<_ValueTy>(min));
		}
	}

	template<Size _BufSize = 8192>
	void stream_copy(std::ostream& dst, std::istream& src, Size byte_count = 0)
	{
		bool limit = byte_count > 0;
		char buffer[_BufSize];
		while (src && dst && (!limit || byte_count > 0))
		{
			Size count = std::min(_BufSize, byte_count);
			src.read(buffer, count);
			count = src.gcount();
			if (limit)
				byte_count = count > byte_count ? 0 : byte_count - count;
			dst.write(buffer, count);
		}
	}
}




namespace utils
{
	class JsonException : std::exception
	{
	public:
		inline JsonException(const char* msg = "") : std::exception{ msg } {}
		inline JsonException(const String& msg) : std::exception{ msg.c_str() } {}
	};

	class JsonSerializable
	{
	public:
		virtual Json serialize() const = 0;
		virtual void deserialize(const Json& json) = 0;
	};

	template<typename _Ty>
	concept JsonSerializableOnly = utils::BaseOf<JsonSerializable, _Ty>;

	Json read(std::istream& input);
	Json read(const Path& path);
	Json read(const String& path);

	void write(std::ostream& output, const Json& json);
	void write(const Path& path, const Json& json);
	void write(const String& path, const Json& json);

	inline Json extract(const JsonSerializable& js) { return js.serialize(); }

	inline void inject(JsonSerializable& js, const Json& json) { js.deserialize(json); }

	inline void read(std::istream& input, JsonSerializable& js) { js.deserialize(read(input)); }
	inline void read(const Path& path, JsonSerializable& js) { js.deserialize(read(path)); }
	inline void read(const String& path, JsonSerializable& js) { js.deserialize(read(path)); }

	inline void write(std::ostream& output, const JsonSerializable& js) { write(output, js.serialize()); }
	inline void write(const Path& path, const JsonSerializable& js) { write(path, js.serialize()); }
	inline void write(const String& path, const JsonSerializable& js) { write(path, js.serialize()); }
}

std::ostream& operator<< (std::ostream& left, const utils::JsonSerializable& right);
std::istream& operator>> (std::istream& left, utils::JsonSerializable& right);

template<utils::JsonSerializableOnly _Ty>
_Ty& operator<< (_Ty& left, const Json& right) { return left.deserialize(right), left; }

template<utils::JsonSerializableOnly _Ty>
_Ty& operator>> (_Ty& left, Json& right) { return right = left.serialize(), left; }




namespace utils
{
	template<typename _Ty>
	concept StringOnly = std::same_as<String, _Ty>;

	template<StringOnly... _Args>
	Path make_path(const _Args&... parts)
	{
		std::vector<String> args = { parts... };
		
		bool start = false;
		std::stringstream ss;
		for (const auto& part : args)
		{
			if (start)
				ss << '/' << part;
			else start = true, ss << part;
		}

		return { ss.str() };
	}
}

inline Path operator"" _p(const char* str, Size size) { return { String{ str, size } }; }

inline Path operator+ (const Path& left, const Path& right) { Path path = left; return path += right; }
inline Path operator+ (const Path& left, const String& right) { Path path = left; return path += right; }
inline Path operator+ (const Path& left, const char* right) { Path path = left; return path += right; }

inline Path& operator<< (Path& left, const Path& right) { return left += right; }
inline Path& operator<< (Path& left, const String& right) { return left += right; }
inline Path& operator<< (Path& left, const char* right) { return left += right; }

inline Path& operator<< (Path&& left, const Path& right) { return left += right; }
inline Path& operator<< (Path&& left, const String& right) { return left += right; }
inline Path& operator<< (Path&& left, const char* right) { return left += right; }

namespace resource
{
	class Folder
	{
	private:
		Path _path;

	public:
		Folder() = default;
		Folder(const Folder&) = default;
		Folder(Folder&&) noexcept = default;
		~Folder() = default;

		Folder& operator= (const Folder&) = default;
		Folder& operator= (Folder&&) noexcept = default;

		bool operator== (const Folder&) const = default;
		auto operator<=> (const Folder&) const = default;

		Folder(const Path& path);
		Folder(const Folder& parent, const Path& path);

		bool openInput(std::ifstream& input);
		bool openInput(const Function<void(std::istream&)>& action);

		bool openOutput(std::ofstream& output);
		bool openOutput(const Function<void(std::ostream&)>& action);

		bool readJson(Json& json);

		bool writeJson(const Json& json);

		template<utils::JsonSerializableOnly _Ty>
		_Ty& readAndInject(_Ty& obj)
		{
			return openInput([&obj](std::istream& in) { utils::read(in, obj); }), obj;
		}

		template<utils::JsonSerializableOnly _Ty>
		void extractAndWrite(_Ty& obj)
		{
			openOutput([&obj](std::ostream& os) { utils::write(os, obj); });
		}

	private:
		bool _open(std::ifstream& stream);
		bool _open(std::ofstream& stream);
	};
}

resource::Folder& operator<< (resource::Folder& left, std::istream& right);
resource::Folder& operator<< (resource::Folder& left, const Json& right);
resource::Folder& operator<< (resource::Folder& left, const utils::JsonSerializable& right);

resource::Folder& operator>> (resource::Folder& left, std::ostream& right);
resource::Folder& operator>> (resource::Folder& left, Json& right);
resource::Folder& operator>> (resource::Folder& left, utils::JsonSerializable& right);
