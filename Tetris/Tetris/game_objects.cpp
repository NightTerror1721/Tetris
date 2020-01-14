#include "game_objects.h"

GameObject::GameObject() :
	_uuid{ UUID::generate() }
{}
GameObject::GameObject(const UUID& uuid) :
	_uuid{ uuid }
{}
GameObject::~GameObject() {}

UUID GameObject::getUUID() const { return _uuid; }

void GameObject::setUUID(const UUID& uuid) { _uuid = uuid; }

bool operator== (const GameObject& obj1, const GameObject& obj2)
{
	return obj1._uuid == obj2._uuid;
}
bool operator!= (const GameObject& obj1, const GameObject& obj2)
{
	return obj1._uuid != obj2._uuid;
}
