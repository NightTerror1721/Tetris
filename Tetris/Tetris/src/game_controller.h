#pragma once

#include "game_basics.h"

enum class WindowStyle : UInt32
{
	None = 0,      ///< No border / title bar (this flag and all others are mutually exclusive)
	Titlebar = 1 << 0, ///< Title bar + fixed border
	Resize = 1 << 1, ///< Title bar + resizable border + maximize button
	Close = 1 << 2, ///< Title bar + close button
	Fullscreen = 1 << 3, ///< Fullscreen mode (this flag and all others are mutually exclusive)

	Default = Titlebar | Resize | Close ///< Default window style
};

class GameController : private GameObjectContainer<GameObject>
{
private:
	bool _close;
	sf::RenderWindow _window;

	sf::Clock _deltaClock;
	sf::Time _phAccumulator;
	sf::Time _phUps;

	std::string _name;
	sf::VideoMode _vmode;
	WindowStyle _wstyle;

public:
	GameController(const GameController&) = delete;
	GameController(GameController&&) noexcept = delete;

	GameController& operator= (const GameController&) = delete;
	GameController& operator= (GameController&&) noexcept = delete;

	GameController(const std::string& name);
	~GameController();

	void start();

	void close();

	void videoMode(sf::VideoMode mode, bool apply = true);
	void style(WindowStyle style, bool apply = true);
	void resetWindow();

	bool fullscreen() const;

private:
	void loop();

	void init();
	void update(const sf::Time& delta);
	void render();
	void processEvents();

protected:
	virtual void onCreate(GameObject& element) override;
	virtual void onDestroy(GameObject& element) override;

public:
	inline GameObjectContainer<GameObject>& objects() { return *this; }
	inline const GameObjectContainer<GameObject>& objects() const { return *this; }
};

namespace global
{
	extern GameController game;
}



constexpr WindowStyle operator& (WindowStyle left, WindowStyle right)
{
	return static_cast<WindowStyle>(static_cast<UInt32>(left) & static_cast<UInt32>(right));
}

constexpr WindowStyle operator| (WindowStyle left, WindowStyle right)
{
	return static_cast<WindowStyle>(static_cast<UInt32>(left) | static_cast<UInt32>(right));
}

constexpr WindowStyle operator~ (WindowStyle right)
{
	return static_cast<WindowStyle>(~static_cast<UInt32>(right));
}

constexpr WindowStyle& operator&= (WindowStyle& left, WindowStyle right)
{
	return left = static_cast<WindowStyle>(static_cast<UInt32>(left) & static_cast<UInt32>(right));
}

constexpr WindowStyle& operator|= (WindowStyle& left, WindowStyle right)
{
	return left = static_cast<WindowStyle>(static_cast<UInt32>(left) | static_cast<UInt32>(right));
}
