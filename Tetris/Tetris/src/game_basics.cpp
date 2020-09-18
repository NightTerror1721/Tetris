#include "game_basics.h"


UID UID::make()
{
	static UInt64 _gen = 0;

	UID uid;
	return uid._value = ++_gen, uid;
}

std::ostream& operator<< (std::ostream& left, const UID& right) { return left << right._value; }
std::istream& operator>> (std::istream& left, UID& right) { return left >> right._value; }

Json& operator<< (Json& left, const UID& right) { return left = right._value, left; }
Json& operator>> (Json& left, UID& right) { return right._value = left.get<UInt64>(), left; }







Frame::Frame(const Vec2u& textureSize, const Vec2f& shapeSize, const Vec2f& shapePosition) :
	_canvas{},
	_shape{}
{
	_canvas.create(textureSize.x, textureSize.y);

	_shape.setPosition(shapePosition);
	_shape.setSize(shapeSize);

	_shape.setTexture(&_canvas.getTexture(), true);
}

void Frame::rebuild(const Vec2u& textureSize, const Vec2f& shapeSize, const Vec2f& shapePosition)
{
	utils::destroy(_canvas);
	utils::construct(_canvas);

	_canvas.create(textureSize.x, textureSize.y);

	_shape.setPosition(shapePosition);
	_shape.setSize(shapeSize);

	_shape.setTexture(&_canvas.getTexture(), true);
}
