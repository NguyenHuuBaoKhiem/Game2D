#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy1.h"
#include "EnemyManager.h"
#include "MenuScreen.h"

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

    EnemyManager enemyManager;

    // --- BOSS ---
    sf::Texture tIdle, tWalk, tAttack, tDeath;
    if (!tIdle.loadFromFile("Assets/Images/Enemy/idle.png") ||
        !tWalk.loadFromFile("Assets/Images/Enemy/walk.png") ||
        !tAttack.loadFromFile("Assets/Images/Enemy/attack.png") ||
        !tDeath.loadFromFile("Assets/Images/Enemy/tile000.png")) {
        return 1;
    }

    auto boss = std::make_unique<Enemy1>(tIdle, tWalk, tAttack, tDeath);
    boss->SetPosition({ 1000.f, 500.f });
    enemyManager.AddEnemy(std::move(boss));

    sf::Clock clock;

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

        window.clear(sf::Color(70, 100, 150)); // nền sáng nhẹ

        // --- MENU ---
        if (menu.isActive) {
            menu.Update(deltaTime, window);
            menu.Draw(window);
        }
        else {
            // --- GAME CHÍNH ---

            // Cập nhật Player
            player.HandleInput(deltaTime);
            player.Update(deltaTime);

            // Lấy vị trí player để boss biết
            sf::Vector2f playerPos = player.GetPosition();

            // Cập nhật Boss
            enemyManager.UpdateAll(deltaTime, playerPos);
            enemyManager.RemoveDeadEnemies();
            // --- Va chạm: Player tấn công Boss ---
            for (const auto& e : enemyManager.GetEnemies()) {
                if ((player.GetState() == PlayerState::Attacking1 ||
                    player.GetState() == PlayerState::Attacking2 ||
                    player.GetState() == PlayerState::Attacking3) &&
                    player.GetAttackBox().intersects(e->GetSprite().getGlobalBounds()))
                {
                    e->TakeDamage(20);
                }
            }

            // --- Va chạm: Boss tấn công Player ---
            // (Nếu bạn muốn boss gây damage)
            // Giả sử Enemy1 có GetAttackBox() thì:
            // if (boss.IsAttacking() && boss.GetAttackBox().intersects(player.GetBodyHitbox()))
            //     player.TakeDamage(15);

            // --- Vẽ ---
            player.Draw(window);
            enemyManager.DrawAll(window);
        }

        window.display();
    }

    return 0;
}
