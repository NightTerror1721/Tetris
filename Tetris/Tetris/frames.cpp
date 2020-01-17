#include "frames.h"

Frame::Frame(const std::string& name) :
	GameObject{},
	GameObjectRepository{ true },
	_name{ name }
{}
Frame::~Frame() {}

void Frame::setName(const std::string& name) { _name = name; }
const std::string& Frame::getName() const { return _name; }
