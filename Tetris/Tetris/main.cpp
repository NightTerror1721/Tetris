#include "game_loop.h"
#include "uuid.h"
#include "mem.h"
#include "frames.h"

#include <iostream>

int main(int argc, char** argv)
{
	GameController gc{ "Tetris" };
	gc.showFps(true);

	gc.createGameObject<Frame>("Test Frame");

	gc.start();

	/*for(int i = 0; i < 9999; ++i)
		std::cout << to_string(UUID::generate()) << std::endl;*/

	/*UUID id = UUID::generate();

	ReadOnlyPointer<UUID> p = &id;
	ConstReadOnlyPointer<UUID> cp = p;

	std::cout << to_string(*cp) << std::endl;

	SimpleLinkedList<int> list;
	list.append(50);
	list.append(-7);
	list.append(64);
	list.erase(50);

	//std::cout << list << std::endl;
	for (int i : list)
		std::cout << i << " ";
	std::cout << std::endl;

	/*UUID uuid = UUID::generate();
	std::cout << to_string(uuid) << std::endl;*/

	return 0;
}