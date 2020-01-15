#pragma once

#include <type_traits>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>

#include "json.h"
#include "uuid.h"
#include "mem.h"

class GameObject
{
private:
	UUID _uuid;

public:
	GameObject();
	GameObject(const UUID& uuid);
	virtual ~GameObject();

	UUID getUUID() const;

	friend bool operator== (const GameObject& obj1, const GameObject& obj2);
	friend bool operator!= (const GameObject& obj1, const GameObject& obj2);

protected:
	void setUUID(const UUID& uuid);
};

bool operator== (const GameObject& obj1, const GameObject& obj2);
bool operator!= (const GameObject& obj1, const GameObject& obj2);



template<class _Ty = GameObject>
class GameObjectRepository
{
	static_assert(std::is_base_of<GameObject, _Ty>::value);

public:
	using Obj = _Ty;
	using ObjPtr = Obj*;
	using Reader = ReadOnlyPointer<Obj>;
	using ConstReader = ConstReadOnlyPointer<Obj>;
	using AllocatorList = SimpleLinkedList<ObjPtr>;
	using Allocator = typename AllocatorList::NodePointer;
	using Entry = std::pair<const UUID, Allocator>;


protected:
	std::map<UUID, Allocator> _objs;
	AllocatorList _list;

public:
	GameObjectRepository() :
		_objs{}
	{}
	virtual ~GameObjectRepository() {}

	bool addGameObject(ObjPtr obj)
	{
		auto uuid = obj->getUUID();
		if (!getGameObject(uuid))
		{
			_list.append(obj);
			_objs[uuid] = _list.lastNode();
			return true;
		}
		return false;
	}

	template<class... _Args>
	Reader createGameObject(_Args&&... args)
	{
		ObjPtr obj = new Obj(args...);
		if (addGameObject(obj))
			return { obj };
		delete obj;
		return nullptr;
	}

	Reader getGameObject(const UUID& uuid)
	{
		const auto& it = _objs.find(uuid);
		return it == _objs.end() ? nullptr : Reader{ *it->second };
	}
	ConstReader getGameObject(const UUID& uuid) const
	{
		const auto& it = _objs.find(uuid);
		return it == _objs.end() ? nullptr : ConstReader{ *it->second };
	}

	inline bool hasGameObject(const UUID& uuid) { return getGameObject(uuid); }

	inline bool hasGameObject(ConstReader obj) { return getGameObject(obj->getUUID()); }

	Reader findGameObject(std::function<bool(Reader)>& predicate)
	{
		Entry* e = _find([&predicate](Entry& e2) -> bool { return predicate(Reader{ *e2.second }); });
		return e ? Reader{ *e->second } : nullptr;
	}
	ConstReader findGameObject(std::function<bool(ConstReader)>& predicate) const
	{
		const Entry* e = _find([&predicate](const Entry& e2) -> bool { return predicate(ConstReader{ *e2.second }); });
		return e ? Reader{ *e->second } : nullptr;
	}

	void forEachGameObject(std::function<void(Reader)>& action)
	{
		for (Entry& e : _objs)
			action(Reader{ *e.second });
	}

	void forEachGameObject(std::function<void(ConstReader)>& action) const
	{
		for (const Entry& e : _objs)
			action(ConstReader{ *e.second });
	}

	bool hasAnyGameObject() const { return !_objs.empty(); }

	size_t getGameObjectCount() const { return _objs.size(); }

	ObjPtr removeGameObject(const UUID& uuid)
	{
		const auto& it = _objs.find(uuid);
		if (it != _objs.end())
		{
			auto alloc = it->second;
			auto ptr = *alloc;
			_objs.erase(it);
			_list.eraseNode(alloc);
			return ptr;
		}
		return nullptr;
	}
	inline ObjPtr removeGameObject(ConstReader obj) { return removeGameObject(obj->getUUID()); }

	bool deleteGameObject(const UUID& uuid)
	{
		ObjPtr p = removeGameObject(uuid);
		if (p)
			delete p;
	}
	inline bool deleteGameObject(ConstReader obj) { return deleteGameObject(obj->getUUID()); }




protected:
	Entry* _find(std::function<bool(Entry&)> predicate)
	{
		const auto& it = std::find_if(_objs.begin(), _objs.end(), predicate);
		return it == _objs.end() ? nullptr : &(*it);
	}
	const Entry* _find(std::function<bool(const Entry&)> predicate) const
	{
		const auto& it = std::find_if(_objs.cbegin(), _objs.cend(), predicate);
		return it == _objs.cend() ? nullptr : &(*it);
	}

	std::vector<Entry*> _findRange(std::function<bool(Entry&)>& predicate)
	{
		std::vector<Entry*> vec{ _objs.size() / 4 };
		for (Entry& e : _objs)
			if (predicate(e))
				vec.push_back(&e);
		return std::move(vec);
	}
	std::vector<const Entry*> _findRange(std::function<bool(const Entry&)>& predicate) const
	{
		std::vector<const Entry*> vec{ _objs.size() / 4 };
		for (const Entry& e : _objs)
			if (predicate(e))
				vec.push_back(&e);
		return std::move(vec);
	}
};
