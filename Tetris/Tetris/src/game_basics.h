#pragma once

#include "common.h"

class UID
{
private:
	UInt64 _value = 0;

public:
	UID() = default;
	UID(const UID&) = default;
	UID(UID&&) noexcept = default;
	~UID() = default;

	UID& operator= (const UID&) = default;
	UID& operator= (UID&&) noexcept = default;

	bool operator== (const UID&) const = default;
	auto operator<=> (const UID&) const = default;

	static UID make();

	friend std::ostream& operator<< (std::ostream& left, const UID& right);
	friend std::istream& operator>> (std::istream& left, UID& right);

	friend Json& operator<< (Json& left, const UID& right);
	friend Json& operator>> (Json& left, UID& right);
};



struct Renderable
{
	virtual void render(sf::RenderTarget& canvas) = 0;
};

struct Updatable
{
	virtual void update(const sf::Time& delta) = 0;
};

struct EventDispatcher
{
	virtual void dispatchEvent(const sf::Event& event) = 0;
};



class GameObject : public Renderable, public Updatable, public EventDispatcher
{
private:
	UID _uid = UID::make();

public:
	GameObject() = default;
	GameObject(const GameObject&) = default;
	GameObject(GameObject&&) noexcept = default;
	virtual ~GameObject() = default;

	GameObject& operator= (const GameObject&) = default;
	GameObject& operator= (GameObject&&) noexcept = default;

	bool operator== (const GameObject&) const = default;

	inline const UID& uid() const { return _uid; }

	virtual void render(sf::RenderTarget& canvas) override {}
	virtual void update(const sf::Time& delta) override {}
	virtual void dispatchEvent(const sf::Event& event) override {}
};



template<typename _Ty>
requires utils::SameOrDerived<GameObject, _Ty>
class GameObjectContainer
{
private:
	using Allocator = utils::Allocator<_Ty>;
	using List = utils::LinkedList<Allocator>;
	using Location = typename List::iterator;
	using Map = std::map<UID, Location>;

private:
	List _objs;
	Map _map;

protected:
	virtual void onCreate(_Ty& element) {}
	virtual void onDestroy(_Ty& element) {}

private:
	template<typename _ObjTy>
	requires utils::SameOrDerived<_Ty, _ObjTy>
	_ObjTy* _alloc(_ObjTy* obj)
	{
		Location loc = _objs.push_back(Allocator::attach(*obj));
		onCreate(*loc->element);
		_map.insert({ loc->element->uid(), loc });

		return obj;
	}

public:
	GameObjectContainer() = default;
	GameObjectContainer(const GameObjectContainer&) = delete;
	GameObjectContainer(GameObjectContainer&&) noexcept = default;
	virtual ~GameObjectContainer() = default;
	
	GameObjectContainer& operator= (const GameObjectContainer&) = delete;
	GameObjectContainer& operator= (GameObjectContainer&&) noexcept = default;
	
	inline bool contains(const UID& uid) const { return _map.find(uid) != _map.end(); }

	template<typename _ObjTy, typename... _Args>
	requires utils::SameOrDerived<_Ty, _ObjTy>
	_ObjTy& emplace(_Args&&... args)
	{
		return *_alloc(new _ObjTy{ std::forward(args)... });
	}

	template<typename _ObjTy>
	requires utils::SameOrDerived<_Ty, _ObjTy>
	_ObjTy* insert(const _ObjTy& obj)
	{
		const UID& uid = obj.uid();
		if (contains(uid))
			return nullptr;

		return _alloc(new _ObjTy{ obj });
	}

	template<typename _ObjTy>
	requires utils::SameOrDerived<_Ty, _ObjTy>
	_ObjTy* insert(_ObjTy&& obj)
	{
		const UID& uid = obj.uid();
		if (contains(uid))
			return nullptr;

		return _alloc(new _ObjTy{ std::move(obj) });
	}

	inline bool empty() const { return _objs.empty(); }
	inline Size size() const { return _objs.size(); }

	inline operator bool() const { return !_objs.empty(); }
	inline bool operator! () const { return _objs.empty(); }

	inline _Ty& get(const UID& uid) { return *_map.at(uid)->element; }
	inline const _Ty& get(const UID& uid) const { return *_map.at(uid)->element; }

	inline _Ty& operator[] (const UID& uid) { return *_map.at(uid)->element; }
	inline const _Ty& operator[] (const UID& uid) const { return *_map.at(uid)->element; }

	bool erase(const UID& uid)
	{
		auto it = _map.find(uid);
		if (it != _map.end())
		{
			Location loc = it->second;
			onDestroy(*loc->element);
			_map.erase(it);
			_objs.erase(loc);
			return true;
		}
		return false;
	}

	void clear()
	{
		_map.clear();
		_objs.clear();
	}

public:
	using iterator = typename List::iterator;
	using const_iterator = typename List::const_iterator;

	inline iterator begin() { return _objs.begin(); }
	inline const_iterator begin() const { return _objs.begin(); }
	inline const_iterator cbegin() const { return _objs.cbegin(); }

	inline iterator end() { return _objs.end(); }
	inline const_iterator end() const { return _objs.end(); }
	inline const_iterator cend() const { return _objs.cend(); }
};



class NoSuchElement : public std::exception
{
public:
	inline NoSuchElement(const char* msg = "") : exception{ msg } {}
	inline NoSuchElement(const String& msg) : exception{ msg.c_str() } {}
};




template<typename _Ty>
class SingleTypeManager
{
private:
	SingleTypeManager* _parent = nullptr;
	std::unordered_map<String, _Ty> _elems;

public:
	SingleTypeManager() = default;
	SingleTypeManager(const SingleTypeManager&) = delete;
	SingleTypeManager(SingleTypeManager&&) noexcept = default;
	virtual ~SingleTypeManager() = default;

	SingleTypeManager& operator= (const SingleTypeManager&) = delete;
	SingleTypeManager& operator= (SingleTypeManager&&) noexcept = default;

protected:
	template<typename... _Args>
	_Ty* create(const String& name, _Args&&... args)
	{
		auto result = _elems.emplace(std::piecewise_construct,
			std::forward_as_tuple(name),
			std::forward_as_tuple(std::forward(args)...)
		);
		if (result.second)
			return &result.first->second;
		return nullptr;
	}

public:
	SingleTypeManager(SingleTypeManager* parent) :
		_parent{ parent },
		_elems{}
	{}

	inline SingleTypeManager* parent() { return _parent; }

	inline operator bool() const { return !_elems.empty(); }
	inline bool operator! () const { return _elems.empty(); }

	inline bool empty() const { return _elems.empty(); }
	inline Size size() const { return _elems.size(); }

	inline bool has(const String& name) const
	{
		return _elems.find(name) != _elems.end() || (_parent && _parent->has(name));
	}

	_Ty& get(const String& name)
	{
		auto it = _elems.find(name);
		if (it != _elems.end())
			return it->second;
		if (!_parent)
			throw NoSuchElement{};
		return _parent->get(name);
	}
	inline const _Ty& get(const String& name) const
	{
		auto it = _elems.find(name);
		if (it != _elems.end())
			return it->second;
		if (!_parent)
			throw NoSuchElement{};
		return _parent->get(name);
	}

	inline bool erase(const String& name) { return _elems.erase(name) != 0; }

	inline void clear() { _elems.clear(); }

	inline _Ty& operator[] (const String& name) { return get(name); }
	inline const _Ty& operator[] (const String& name) const { return get(name); }

	inline _Ty& operator[] (const char* name) { return get(String{ name }); }
	inline const _Ty& operator[] (const char* name) const { return get(String{ name }); }
};



class Frame
{
private:
	sf::RectangleShape _shape;
	sf::RenderTexture _canvas;

public:
	Frame(const Vec2u& textureSize, const Vec2f& shapeSize = {}, const Vec2f& shapePosition = {});
	Frame(const Frame&) = delete;
	Frame(Frame&&) noexcept = default;
	virtual ~Frame() = default;

	Frame& operator= (const Frame&) = delete;
	Frame& operator= (Frame&&) noexcept = default;

	inline const Vec2f& getPosition() const { return _shape.getPosition(); }
	inline const Vec2f& getSize() const { return _shape.getSize(); }

	inline void setPosition(const Vec2f& position) { _shape.setPosition(position); }
	inline void setSize(const Vec2f& size) { _shape.setSize(size); }

	inline Vec2u getCanvasSize() const { return _canvas.getSize(); }

	inline void setPerimeterColor(const sf::Color& color) { _shape.setOutlineColor(color); }
	inline void setPerimeterThickness(float thickness) { _shape.setOutlineThickness(thickness); }

protected:
	void rebuild(const Vec2u& textureSize, const Vec2f& shapeSize = {}, const Vec2f& shapePosition = {});

	inline void clearCanvas() { _canvas.clear(); }
	inline void displayCanvas() { _canvas.display(); }
	inline void renderCanvas(sf::RenderTarget& canvas, bool display = true) { if (display) _canvas.display(); canvas.draw(_shape); }
	inline void draw(const sf::Drawable& drawable) { _canvas.draw(drawable); }
	inline sf::RenderTarget& canvas() { return _canvas; }
};
