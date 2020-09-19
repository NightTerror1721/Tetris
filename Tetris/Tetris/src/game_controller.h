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

class FPSMonitor
{
private:
	static constexpr Int64 identity = 1000000;

	Int64 _remaining = identity;
	unsigned int _current = 0;
	unsigned int _last = 0;
	bool _enabled = false;

	sf::Text _text;

public:
	void init();

	void update(const sf::Time& delta);
	void render(sf::RenderTarget& canvas);

	inline bool enabled() const { return _enabled; }
	inline void enabled(bool flag) { _enabled = flag; }
};

class GameController : private GameObjectContainer<GameObject>
{
public:
	static constexpr int canvas_width = utils::game_canvas_with;
	static constexpr int canvas_height = utils::game_canvas_height;

private:
	bool _close;
	sf::RenderWindow _window;

	sf::Clock _deltaClock;
	sf::Time _phAccumulator;
	sf::Time _phUps;

	std::string _name;
	sf::VideoMode _vmode;
	WindowStyle _wstyle;

	sf::RenderTexture _virtualCanvas;
	sf::RectangleShape _virtualWindow;
	sf::View _view;

	FPSMonitor _fps;

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
	void update();
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
