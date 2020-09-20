#include "common.h"
#include "game_controller.h"
#include "theme.h"
#include "scenario.h"
#include "fonts.h"


struct Tester : public GameObject
{
	Scenario scenario;

	Tester() : GameObject{} {};

	void render(sf::RenderTarget& canvas) override
	{
		scenario.render(canvas);
	}
	void update(const sf::Time& delta) override
	{
		scenario.update(delta);
	}
	void dispatchEvent(const sf::Event& event) override
	{
		scenario.dispatchEvent(event);
	}
};


int main(int argc, char** argv)
{
	global::theme.load("default");

	Tester& tester = global::game.objects().emplace<Tester>();

	tester.scenario.setPosition({
		(utils::game_canvas_with / 2) - (Scenario::width / 2),
		(utils::game_canvas_height / 2) - (Scenario::height / 2)
	});

	tester.scenario.setPerimeterColor(sf::Color::Blue);
	tester.scenario.setPerimeterThickness(3);

	tester.scenario.field().setPerimeterColor(sf::Color::Blue);
	tester.scenario.field().setPerimeterThickness(3);

	tester.scenario.nextTetrominoManager().setPerimeterColor(sf::Color::Blue);
	tester.scenario.nextTetrominoManager().setPerimeterThickness(3);

	tester.scenario.holdManager().setPerimeterColor(sf::Color::Blue);
	tester.scenario.holdManager().setPerimeterThickness(3);

	global::game.videoMode({ 1600, 900 });
	
	global::game.start();

	return 0;
}