#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"
#include "EnemyManager.h"
#include "MenuScreen.h"
#include "Map.h"
#include <vector>
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 896), "Game", sf::Style::Close);
    window.setFramerateLimit(60);

    // --- MAP ---
    Map map;
    std::vector<std::string> levelNames = { "Level_0", "Level_1", "Level_2", "Level_3" };
    int currentLevel = 0;
    map.Load("Assets/Images/Map/Map.ldtk", levelNames[currentLevel]);

    sf::Music bgMusic;
    bool mapMusicStarted = false;
    std::vector<std::string> musicPaths = {
        "Assets/Music/intro.mp3",
        "Assets/Music/boss2.mp3",
        "Assets/Music/boss22.mp3",
        "Assets/Music/boss3.mp3"
    };

    // --- PLAYER ---
    sf::Texture texIdle, texWalk, texAttack1, texAttack2, texAttack3, texSkill1, texDeath;
    texWalk.loadFromFile("Assets/Images/Player/run.png");
    texIdle.loadFromFile("Assets/Images/Player/idle.png");
    texAttack1.loadFromFile("Assets/Images/Player/at1.png");
    texAttack2.loadFromFile("Assets/Images/Player/at2.png");
    texAttack3.loadFromFile("Assets/Images/Player/at3.png");
    texSkill1.loadFromFile("Assets/Images/Player/skill1.png");
    texDeath.loadFromFile("Assets/Images/Player/death.png");

    Player player(texIdle, texWalk, texAttack1, texAttack2, texAttack3, texSkill1, texDeath);
    player.SetPosition({ 200.f, 655.f });

    // --- ENEMY MANAGER ---
    EnemyManager enemyManager;

    // --- MENU ---
    MenuScreen menu(window);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (menu.isActive && event.type == sf::Event::KeyPressed)
                menu.isFadingOut = true;
        }

        float deltaTime = clock.restart().asSeconds();
        window.clear(sf::Color(70, 100, 150));

        // --- MENU ---
        if (menu.isActive) {
            menu.Update(deltaTime, window);
            menu.Draw(window);
        }
        else {
            // --- GAME ---
            if (!mapMusicStarted) {
                mapMusicStarted = true;
                if (!musicPaths[currentLevel].empty() && bgMusic.openFromFile(musicPaths[currentLevel])) {
                    bgMusic.setLoop(true);
                    bgMusic.setVolume(20.f);
                    bgMusic.play();
                }
            }

            // Cập nhật Player
            player.HandleInput(deltaTime);
            player.Update(deltaTime);

            // Giới hạn biên map
            sf::Vector2f pos = player.GetPosition();
            if (pos.x < 50.f) pos.x = 50.f;
            if (pos.x > 1550.f) pos.x = 1550.f;
            player.SetPosition(pos);

            sf::Vector2f playerPos = player.GetPosition();

            // Cập nhật Boss
            enemyManager.UpdateAll(deltaTime, playerPos);
            enemyManager.RemoveDeadEnemies();

            // Va chạm Player → Boss
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

            // Va chạm Boss → Player
            for (const auto& e : enemyManager.GetEnemies()) {
                if (e->IsAttacking() && e->GetAttackBox().intersects(player.GetBodyHitbox())) {
                    bool inSafe = false;
                    const auto& safeZones = e->GetSafeZones(); // chỉ Boss3
                    for (const auto& zone : safeZones) {
                        if (zone.intersects(player.GetBodyHitbox())) {
                            inSafe = true;
                            break;
                        }
                    }
                    if (!inSafe)
                        player.TakeDamage(15);
                }
            }

            // --- Kiểm tra boss chết ---
            bool allDead = true;
            for (const auto& e : enemyManager.GetEnemies())
                if (!e->IsDead()) allDead = false;

            // --- Chuyển map khi đi đến cuối ---
            if (pos.x >= 1550.f && (currentLevel == 0 || allDead)) {
                if (!menu.IsMapFadingOut() && !menu.IsMapFadingIn() && currentLevel + 1 < levelNames.size()) {
                    menu.StartFadeOutMap([&]() {
                        // Callback khi fade-out xong → load map mới
                        currentLevel++;
                        if (currentLevel < levelNames.size()) {
                            // Load map mới
                            map.Load("Assets/Images/Map/Map.ldtk", levelNames[currentLevel]);
                            player.SetPosition({ 200.f, 650.f });
                            enemyManager = EnemyManager();
                            mapMusicStarted = false;

                            // Spawn boss theo map mới
                            if (currentLevel == 1) {
                                static sf::Texture tIdlee, tWalkk, tAttackk, tDeathh;
                                static bool loaded = false;
                                if (!loaded) {
                                    tIdlee.loadFromFile("Assets/Images/Enemy/idle_1.png");
                                    tWalkk.loadFromFile("Assets/Images/Enemy/walk_1.png");
                                    tAttackk.loadFromFile("Assets/Images/Enemy/attack_1.png");
                                    tDeathh.loadFromFile("Assets/Images/Enemy/death_1.png");
                                    loaded = true;
                                }
                                auto boss = std::make_unique<Enemy2>(tIdlee, tWalkk, tAttackk, tDeathh);
                                boss->SetPosition({ 1000.f, 610.f });
                                enemyManager.AddEnemy(std::move(boss));
                            }
                            else if (currentLevel == 2) {
                                static sf::Texture tIdle, tWalk, tAttack, tDeath, tAttack1, tAttack2;
                                static bool loaded = false;
                                if (!loaded) {
                                    tIdle.loadFromFile("Assets/Images/Enemy/idle.png");
                                    tWalk.loadFromFile("Assets/Images/Enemy/walk.png");
                                    tAttack.loadFromFile("Assets/Images/Enemy/attack.png");
                                    tAttack1.loadFromFile("Assets/Images/Enemy/at1.png");
                                    tAttack2.loadFromFile("Assets/Images/Enemy/at2.png");
                                    tDeath.loadFromFile("Assets/Images/Enemy/death.png");
                                    loaded = true;
                                }
                                auto boss = std::make_unique<Enemy1>(tIdle, tWalk, tAttack, tAttack1, tAttack2, tDeath);
                                boss->SetPosition({ 1000.f, 550.f });
                                enemyManager.AddEnemy(std::move(boss));
                            }
                            else if (currentLevel == 3) {
                                static sf::Texture tIdle3, tWalk3, tAttack3, tAttack13, tAttack23, tTele3, tDeath3;
                                static bool loaded = false;
                                if (!loaded) {
                                    tIdle3.loadFromFile("Assets/Images/Enemy/idle_2.png");
                                    tWalk3.loadFromFile("Assets/Images/Enemy/walk_2.png");
                                    tAttack3.loadFromFile("Assets/Images/Enemy/attack_2.png");
                                    tAttack13.loadFromFile("Assets/Images/Enemy/at1_2.png");
                                    tAttack23.loadFromFile("Assets/Images/Enemy/at2_2.png");
                                    tTele3.loadFromFile("Assets/Images/Enemy/tele.png");
                                    tDeath3.loadFromFile("Assets/Images/Enemy/death_2.png");
                                    loaded = true;
                                }
                                auto boss = std::make_unique<Enemy3>(tIdle3, tWalk3, tAttack3, tAttack13, tAttack23, tTele3, tDeath3);
                                boss->SetPosition({ 830.f, 380.f });
                                enemyManager.AddEnemy(std::move(boss));
                            }
                        }
                     },2.5f);
                }
            }

            // --- Cập nhật fade map ---
            menu.UpdateMapFade(deltaTime);

            // --- Vẽ ---
            map.Draw(window);
            player.Draw(window);
            enemyManager.DrawAll(window);

            // Vẽ hiệu ứng fade map
            menu.DrawMapFade(window);

            // Vẽ HP Boss
            for (const auto& e : enemyManager.GetEnemies())
                e->DrawHP(window);

        }

        window.display();
    }

    return 0;
}
