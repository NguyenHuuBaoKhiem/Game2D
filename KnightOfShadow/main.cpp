#include<SFML/Graphics.hpp>
#include"Player.h"
int main()
{
	sf::RenderWindow window(sf::VideoMode(1600, 800), "Game");
	window.setFramerateLimit(60);

	sf::Texture texIdle, texWalk, texAttack1, texAttack2, texAttack3;
	texWalk.loadFromFile("Assets/Images/Player/run.png");
	texIdle.loadFromFile("Assets/Images/Player/idle.png");
	texAttack1.loadFromFile("Assets/Images/Player/at1.png");
	texAttack2.loadFromFile("Assets/Images/Player/at2.png");
	texAttack3.loadFromFile("Assets/Images/Player/at3.png");

	texIdle.setSmooth(true);
	texWalk.setSmooth(true);
	texAttack1.setSmooth(true);
	texAttack2.setSmooth(true);
	texAttack3.setSmooth(true);

	Player player(texIdle, texWalk, texAttack1, texAttack2, texAttack3);

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		float deltaTime = clock.restart().asSeconds();

		player.HandleInput(deltaTime);
		player.Update(deltaTime);

		window.clear(sf::Color::White);
		player.Draw(window);
		window.display();
	}

	return 0;

}