#include "common.h"




namespace utils
{
	Json read(std::istream& input)
	{
		try
		{
			Json json;
			input >> json;
			return json;
		}
		catch (const std::exception& ex) { throw JsonException{ ex.what() }; }
	}
	Json read(const Path& path)
	{
		std::fstream f{ path, std::ios::in };
		return read(f);
	}
	Json read(const String& path)
	{
		std::fstream f{ path, std::ios::in };
		return read(f);
	}

	void write(std::ostream& output, const Json& json)
	{
		try
		{
			output << json;
		}
		catch (const std::exception& ex) { throw JsonException{ ex.what() }; }
	}
	void write(const Path& path, const Json& json)
	{
		std::fstream f{ path, std::ios::out };
		write(f, json);
	}
	void write(const String& path, const Json& json)
	{
		std::fstream f{ path, std::ios::out };
		write(f, json);
	}
}

std::ostream& operator<< (std::ostream& left, const utils::JsonSerializable& right) { return utils::write(left, right), left; }
std::istream& operator>> (std::istream& left, utils::JsonSerializable& right) { return utils::read(left, right), left; }





namespace resource
{
	Folder::Folder(const Path& path) :
		_path{ path }
	{}
	Folder::Folder(const Folder& parent, const Path& path) :
		_path{ parent._path + path }
	{}

	bool Folder::_open(std::ifstream& stream)
	{
		stream.open(_path, std::ios::in);
		return !stream.fail();
	}
	bool Folder::_open(std::ofstream& stream)
	{
		stream.open(_path, std::ios::out);
		return !stream.fail();
	}

	bool Folder::openInput(std::ifstream& input) { return _open(input); }
	bool Folder::openInput(const Function<void(std::istream&)>& action)
	{
		std::ifstream stream;
		if (_open(stream))
			return action(stream), true;
		return false;
	}

	bool Folder::openOutput(std::ofstream& output) { return _open(output); }
	bool Folder::openOutput(const Function<void(std::ostream&)>& action)
	{
		std::ofstream stream;
		if (_open(stream))
			return action(stream), true;
		return false;
	}

	bool Folder::readJson(Json& json) { return openInput([&json](std::istream& is) { json = utils::read(is); }); }

	bool Folder::writeJson(const Json& json) { return openOutput([&json](std::ostream& os) { utils::write(os, json); }); }
}

resource::Folder& operator<< (resource::Folder& left, std::istream& right)
{
	if (right)
		left.openOutput([&right](std::ostream& os) { utils::stream_copy(os, right); });
	return left;
}
resource::Folder& operator<< (resource::Folder& left, const Json& right) { return left.writeJson(right), left; }
resource::Folder& operator<< (resource::Folder& left, const utils::JsonSerializable& right) { return left.extractAndWrite(right), left; }

resource::Folder& operator>> (resource::Folder& left, std::ostream& right)
{
	if (right)
		left.openInput([&right](std::istream& is) { utils::stream_copy(right, is); });
	return left;
}
resource::Folder& operator>> (resource::Folder& left, Json& right) { return left.readJson(right), left; }
resource::Folder& operator>> (resource::Folder& left, utils::JsonSerializable& right) { return left.readAndInject(right), left; }
