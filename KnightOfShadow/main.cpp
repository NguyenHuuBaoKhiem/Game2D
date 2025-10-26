#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy.h"
#include "MenuScreen.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 896), "Game", sf::Style::Close);
    window.setFramerateLimit(60);

    // --- Load texture ---
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

    // --- Entities ---
    Player player(texIdle, texWalk, texAttack1, texAttack2, texAttack3);

    // --- Menu ---
    MenuScreen menu(window);

    sf::Clock clock;

    // --- Main loop ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Nhấn bất kỳ phím nào để thoát menu
            if (menu.isActive && event.type == sf::Event::KeyPressed)
                menu.isFadingOut = true;
        }

        float deltaTime = clock.restart().asSeconds();

        window.clear(sf::Color::White);

        // --- Nếu menu đang bật ---
        if (menu.isActive) {
            menu.Update(deltaTime, window);
            menu.Draw(window);
        }
        else {
            // --- Chạy game chính ---
            player.HandleInput(deltaTime);
            player.Update(deltaTime);
            player.Draw(window);
        }

        window.display();
    }

    return 0;
}
