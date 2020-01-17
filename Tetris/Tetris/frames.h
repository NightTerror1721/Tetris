#pragma once

#include "game_objects.h"

class Frame : public GameObject, public GameObjectRepository<>
{
private:
	std::string _name;

public:
	Frame(const std::string& name);
	virtual ~Frame();

	void setName(const std::string& name);
	const std::string& getName() const;
};
