#include "game_loop.h"
#include "uuid.h"

#include <iostream>

int main(int argc, char** argv)
{
	/*GameController gc{ "Tetris" };
	gc.showFps(true);

	gc.start();*/

	for(int i = 0; i < 9999; ++i)
		std::cout << to_string(UUID::generate()) << std::endl;

	/*UUID uuid = UUID::generate();
	std::cout << to_string(uuid) << std::endl;*/

	return 0;
}