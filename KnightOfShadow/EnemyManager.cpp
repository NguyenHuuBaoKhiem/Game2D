#include "EnemyManager.h"

void EnemyManager::AddEnemy(std::unique_ptr<BaseEnemy> enemy) {
    enemies.push_back(std::move(enemy));
}

void EnemyManager::UpdateAll(float deltaTime, const sf::Vector2f& playerPos) {
    for (auto& e : enemies) {
        // Nếu là Enemy1 thì gọi HandleInput riêng (để giữ logic cũ)
        if (auto enemy1 = dynamic_cast<Enemy1*>(e.get())) {
            enemy1->HandleInput(deltaTime, playerPos);
        }
        else if (auto enemy2 = dynamic_cast<Enemy2*>(e.get())) {
            enemy2->HandleInput(deltaTime, playerPos);
        }
        e->Update(deltaTime);
    }

    RemoveDeadEnemies();
}

void EnemyManager::DrawAll(sf::RenderWindow& window) {
    for (auto& e : enemies) {
        e->Draw(window);
    }
}

void EnemyManager::RemoveDeadEnemies() {
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<BaseEnemy>& e) {
                return e->IsDead() && e->IsDeathAnimFinished(); // chỉ xóa khi anim chết xong
            }),
        enemies.end()
    );
}
