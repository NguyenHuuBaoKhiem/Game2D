#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy.h"
#include "MenuScreen.h"
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 896), "Game", sf::Style::Close);
    window.setFramerateLimit(60);

    // --- MENU ---
    MenuScreen menu(window);

    // --- PLAYER ---
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
    player.SetPosition(sf::Vector2f(200.f, 500.f));

    // --- BOSS (Enemy) ---
    sf::Texture tIdle, tWalk, tAttack, tHit, tDeath;
    if (!tIdle.loadFromFile("Assets/images/Enemy/idle.png") ||
        !tWalk.loadFromFile("Assets/images/Enemy/walk.png") ||
        !tAttack.loadFromFile("Assets/images/Enemy/attack.png") ||
        !tHit.loadFromFile("Assets/images/Enemy/takehit.png") ||
        !tDeath.loadFromFile("Assets/images/Enemy/death.png")) {
        std::cerr << "❌ KHONG LOAD DUOC ANH! Kiem tra file PNG trong thu muc.\n";
        return 1;
    }

    Enemy boss(tIdle, tWalk, tAttack, tHit, tDeath);
    boss.SetPosition(sf::Vector2f(1000.f, 500.f));

    sf::Clock clock;
    bool bossAttackLogged = false;

    // --- MAIN LOOP ---
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

        window.clear(sf::Color(220, 220, 255)); // nền sáng nhẹ

        // --- MENU ---
        if (menu.isActive) {
            menu.Update(deltaTime, window);
            menu.Draw(window);
        }
        else {
            // --- GAME CHÍNH ---
            player.HandleInput(deltaTime);
            player.Update(deltaTime);

            // Lấy vị trí player để boss biết mà di chuyển/tấn công
            sf::Vector2f playerPos = player.GetPosition();

            boss.HandleInput(deltaTime, playerPos);
            boss.Update(deltaTime);

            // --- Kiểm tra va chạm boss vs player ---
            if (boss.GetAttackBox().intersects(player.GetGlobalBounds())) {
                if (!bossAttackLogged) {
                    bossAttackLogged = true;
                }
            }
            else {
                bossAttackLogged = false;
            }

            // --- Vẽ ---
            player.Draw(window);
            boss.Draw(window);
        }

        window.display();
    }

    return 0;
}
