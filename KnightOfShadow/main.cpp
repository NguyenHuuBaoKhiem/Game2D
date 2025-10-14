#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "Player.h"
#include "Enemy.h"
#include "Map.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main()
{
    // Tạo cửa sổ
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Knight Of Shadow");
    window.setFramerateLimit(60);

    // Load âm thanh nền (nếu có)
    sf::Music bgMusic;
    if (!bgMusic.openFromFile("Assets/sounds/background.ogg")) {
        std::cout << "Không thể mở file nhạc!\n";
    }
    else {
        bgMusic.setLoop(true);
        bgMusic.play();
    }

    // Tạo đối tượng
    Player player;
    Enemy enemy;
    Map map;

    // Vòng lặp game chính
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Gửi sự kiện điều khiển cho Player
            player.handleEvent(event);
        }

        // Cập nhật logic
        player.update();
        enemy.update();

        // Vẽ
        window.clear(sf::Color::Black);
        map.draw(window);
        enemy.draw(window);
        player.draw(window);
        window.display();
    }

    return 0;
}
