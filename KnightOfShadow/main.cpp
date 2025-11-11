#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"
#include "EnemyManager.h"
#include "MenuScreen.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 896), "Game", sf::Style::Close);
    window.setFramerateLimit(60);

    // --- MENU ---
    MenuScreen menu(window);

    // --- MAP ---
    //Map map;
    //map.Load("Assets/Images/Map/Map.ldtk", "Level_0");

    // --- PLAYER ---
    sf::Texture texIdle, texWalk, texAttack1, texAttack2, texAttack3, texSkill1;
    texWalk.loadFromFile("Assets/Images/Player/run.png");
    texIdle.loadFromFile("Assets/Images/Player/idle.png");
    texAttack1.loadFromFile("Assets/Images/Player/at1.png");
    texAttack2.loadFromFile("Assets/Images/Player/at2.png");
    texAttack3.loadFromFile("Assets/Images/Player/at3.png");
    texSkill1.loadFromFile("Assets/Images/Player/skill1.png");

    texIdle.setSmooth(true);
    texWalk.setSmooth(true);
    texAttack1.setSmooth(true);
    texAttack2.setSmooth(true);
    texAttack3.setSmooth(true);
    texSkill1.setSmooth(true);

    Player player(texIdle, texWalk, texAttack1, texAttack2, texAttack3, texSkill1);
    player.SetPosition(sf::Vector2f(200.f, 500.f));

    // --- ENEMY MANAGER ---
    EnemyManager enemyManager;

    //// --- BOSS 1 ---
    sf::Texture tIdle, tWalk, tAttack, tDeath, tAttack1, tAttack2;
    tIdle.loadFromFile("Assets/Images/Enemy/idle.png");
    tWalk.loadFromFile("Assets/Images/Enemy/walk.png");
    tAttack.loadFromFile("Assets/Images/Enemy/attack.png");
    tAttack1.loadFromFile("Assets/Images/Enemy/at1.png");
    tAttack2.loadFromFile("Assets/Images/Enemy/at2.png");
    tDeath.loadFromFile("Assets/Images/Enemy/death.png");

    auto boss1 = std::make_unique<Enemy1>(tIdle, tWalk, tAttack, tAttack1, tAttack2, tDeath);
    boss1->SetPosition({ 700.f, 500.f });
    enemyManager.AddEnemy(std::move(boss1));

    //// --- BOSS 2 ---
    sf::Texture tIdlee, tWalkk, tAttackk, tDeathh;
    tIdlee.loadFromFile("Assets/Images/Enemy/idle_1.png");
    tWalkk.loadFromFile("Assets/Images/Enemy/walk_1.png");
    tAttackk.loadFromFile("Assets/Images/Enemy/attack_1.png");
    tDeathh.loadFromFile("Assets/Images/Enemy/death_1.png");

    auto boss2 = std::make_unique<Enemy2>(tIdlee, tWalkk, tAttackk, tDeathh);
    boss2->SetPosition({ 1000.f, 500.f });
    enemyManager.AddEnemy(std::move(boss2));

    // --- BOSS 3 ---
    sf::Texture tIdle3, tWalk3, tAttack3, tAttack13, tAttack23, tDeath3;
    tIdle3.loadFromFile("Assets/Images/Enemy/idle_2.png");
    tWalk3.loadFromFile("Assets/Images/Enemy/walk_2.png");
    tAttack3.loadFromFile("Assets/Images/Enemy/attack_2.png");
    tAttack13.loadFromFile("Assets/Images/Enemy/at1_2.png");
    tAttack23.loadFromFile("Assets/Images/Enemy/at2_2.png");
    tDeath3.loadFromFile("Assets/Images/Enemy/death_2.png");

    auto boss3 = std::make_unique<Enemy3>(tIdle3, tWalk3, tAttack3, tAttack13, tAttack23, tDeath3);
    boss3->SetPosition({ 1300.f, 500.f });
    enemyManager.AddEnemy(std::move(boss3));

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
                    player.GetAttackBox().intersects(e->GetBodyHitbox()))
                {
                    e->TakeDamage(20);
                }
                if (player.GetState() == PlayerState::Skill1 &&
                    player.GetSkill1Hitbox().intersects(e->GetBodyHitbox()))
                {
                    e->TakeDamage(50);
                }
            }

            for (const auto& e : enemyManager.GetEnemies())
            {
                // chỉ khi boss đang đánh và hitbox đang bật
                if (e->IsAttacking() && e->GetAttackBox().intersects(player.GetBodyHitbox()))
                {
                    player.TakeDamage(15); // hoặc tuỳ từng skill
                }
            }

            player.Draw(window);
            enemyManager.DrawAll(window);
        }

        window.display();
    }

    return 0;
}