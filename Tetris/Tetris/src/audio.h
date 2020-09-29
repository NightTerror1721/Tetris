#pragma once

#include "game_basics.h"

using sf::Sound;
using sf::Music;

class SoundManager : public SingleTypeManager<Sound>
{
private:
	std::map<String, sf::SoundBuffer> _buffers;

private:
	sf::SoundBuffer& _getBuffer(const Path& path);
	Sound& _load(const Path& filepath, const String& name);

public:
	inline Sound& load(const String& filename, const String& name) { return _load(filename, name); }
	inline Sound& load(const Path& path, const String& name) { return _load(path, name); }

	void loadAll();
};

namespace global { extern SoundManager sounds; }



class SoundController
{
private:
	struct SoundHandle
	{
		Sound* sound;
		bool play;
	};

private:
	std::map<String, SoundHandle> _sounds;

public:
	SoundController() = default;
	SoundController(const SoundController&) = default;
	SoundController(SoundController&&) noexcept = default;
	~SoundController() = default;

	SoundController& operator= (const SoundController&) = default;
	SoundController& operator= (SoundController&&) noexcept = default;

	void update();

	void play(const String& soundName);

	inline void play(const char* soundName) { play(String{ soundName }); }
};



struct MusicInfo
{
	String file;
	bool loop;
	UInt64 start;
	UInt64 end;
};

namespace musics
{
	MusicInfo readInfo(const Json& json);
	Music& load(Music& music, const MusicInfo& info, const resource::Folder& folder);
}

namespace global::musics
{
	void prepareCache();

	Music& load(Music& music, const String& name);
}



namespace sound_id
{
	constexpr const char* single_line = "single_line";
	constexpr const char* double_line = "double_line";
	constexpr const char* triple_line = "triple_line";
	constexpr const char* tetris_line = "tetris_line";
	constexpr const char* all_clear = "all_clear";
	constexpr const char* special_clear = "special_clear";

	constexpr const char* drop_after_clear = "drop_after_clear";

	constexpr const char* tetrimino_move = "tetrimino_move";
	constexpr const char* tetrimino_rotate = "tetrimino_rotate";
	constexpr const char* tetrimino_hold = "tetrimino_hold";
	constexpr const char* tetrimino_hit = "tetrimino_hit";
	constexpr const char* tetrimino_softdrop = "tetrimino_softdrop";
	constexpr const char* tetrimino_harddrop = "tetrimino_harddrop";

	constexpr const char* number_count = "number_count";
}
