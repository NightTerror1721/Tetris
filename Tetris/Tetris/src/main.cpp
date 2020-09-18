#include "common.h"
#include "game_controller.h"
#include "theme.h"
#include "scenario.h"
#include "fonts.h"


struct Tester : public GameObject
{
	Field field;

	Tester() : GameObject{} {};

	void render(sf::RenderTarget& canvas) override
	{
		field.render(canvas);
	}
	void update(const sf::Time& delta) override
	{
		field.update(delta);
	}
	void dispatchEvent(const sf::Event& event) override {}
};


int main(int argc, char** argv)
{
	global::theme.load("default");

	Tester& tester = global::game.objects().emplace<Tester>();

	tester.field.cell(0, 0).changeColor(CellColor::Red);
	tester.field.cell(0, 1).changeColor(CellColor::Red);
	tester.field.cell(1, 0).changeColor(CellColor::Red);

	global::game.videoMode({ 1600, 900 });
	
	global::game.start();

	return 0;
}