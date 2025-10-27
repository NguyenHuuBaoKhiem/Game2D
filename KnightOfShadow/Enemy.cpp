#include "Enemy.h"
#include <cmath> 
#include <iostream> 


std::vector<EnemyTrail> enemyTrails; 
float trailSpawnTimer = 0.f; 
float trailSpawnInterval = 0.025f;


Enemy::Enemy(float x, float y, sf::Vector2u screenSize)
    : screenSize(screenSize),
    idleAnim("Assets/images/boss/idle.png", { 6, 1 }, 0.1f),
    walkAnim("Assets/images/boss/walk.png", { 12, 1 }, 0.1f),)
    attackAnim("Assets/images/boss/attack.png", { 15, 1 }, 0.1f), 
    takeHitAnim("Assets/images/boss/takehit.png", { 5, 1 }, 0.1f),
    deathAnim("Assets/images/boss/death.png", { 22, 1 }, 0.15f) {

    currentAnim = &idleAnim; 
    if (currentAnim && currentAnim->getTexture()) { 
        sprite.setTexture(*currentAnim->getTexture()); // Gán texture cho sprite
        sprite.setTextureRect(currentAnim->GetRect()); // Đặt frame đầu tiên
        sprite.setPosition(x, y); // Đặt vị trí ban đầu
        sprite.setOrigin(432.f, 240.f); 
        sprite.setScale(0.5f, 0.5f);
    }
    else {
        std::cerr << "Error: No valid animation for Enemy initialization!" << std::endl;
    }

//    // Load âm thanh cho các hành động
//    if (!attackBuffer.loadFromFile("Assets/images/boss/attack.wav")) { // Load âm thanh tấn công
//        std::cerr << "Error: Failed to load Assets/images/boss/attack.wav!" << std::endl;
//    }
//    else {
//        attackSound.setBuffer(attackBuffer); // Gán buffer vào âm thanh
//        attackSound.setVolume(50.f); // Đặt âm lượng
//    }
//
//    if (!hitBuffer.loadFromFile("Assets/images/boss/hit.wav")) { // Load âm thanh khi bị đánh
//        std::cerr << "Error: Failed to load Assets/images/boss/hit.wav!" << std::endl;
//    }
//    else {
//        hitSound.setBuffer(hitBuffer);
//        hitSound.setVolume(60.f);
//    }
//
//    if (!deathBuffer.loadFromFile("Assets/images/boss/death.wav")) { // Load âm thanh khi chết
//        std::cerr << "Error: Failed to load Assets/images/boss/death.wav!" << std::endl;
//    }
//    else {
//        deathSound.setBuffer(deathBuffer);
//        deathSound.setVolume(70.f);
//    }
//}

Enemy::~Enemy() {}

// Cập nhật trạng thái của enemy
void Enemy::Update(float deltaTime, const sf::Vector2f& playerPos) {
    if (!currentAnim || !currentAnim->getTexture()) return;

    currentAnim->Update(deltaTime, state == Idle || state == Walk);
    sprite.setTextureRect(currentAnim->GetRect());
    if (attackCooldown > 0) attackCooldown -= deltaTime;
    if (hitTimer > 0) {
        hitTimer -= deltaTime;
        if (hitTimer <= 0) ChangeState(Idle);
    }

    if (state != Death) { // Nếu enemy chưa chết
        float distance = getDistanceToPlayer(playerPos); 
        if (distance <= detectionRange) { 
            moveTowardsPlayer(deltaTime, playerPos); 
            if (distance <= attackRange) performAttack(); 
        }
        else if (state != Attack && state != TakeHit) { // Nếu ngoài tầm và không tấn công/bị đánh
            ChangeState(Idle); // Chuyển về Idle
        }
    }

    if (hp <= 0 && state != Death) {
        ChangeState(Death);
        if (deathSound.getStatus() != sf::Sound::Playing) deathSound.play(); // Phát âm thanh chết
    }

    updateHitbox(); // Cập nhật hitbox
    if (state == TakeHit) sprite.setColor(sf::Color(255, 100, 100));
    else sprite.setColor(sf::Color::White);
}

// Vẽ enemy và hiệu ứng lên màn hình
void Enemy::Draw(sf::RenderWindow& window) {
    if (!currentAnim || !currentAnim->getTexture()) return;

    for (auto& t : enemyTrails) {
        t.sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(t.alpha)));
        window.draw(t.sprite);
        t.alpha -= 500.f * trailSpawnInterval;
        if (t.alpha <= 0) {
            auto it = std::find_if(enemyTrails.begin(), enemyTrails.end(), [&t](const EnemyTrail& bt) { return &bt == &t; });
            if (it != enemyTrails.end()) enemyTrails.erase(it);
        }
    }
    window.draw(sprite);
    drawHPBar(window, sf::RenderStates::Default);
}

// Nhận damage từ player (fix sau)
void Enemy::TakeDamage(int amount) {
    if (state != Death && hp > 0) { // Nếu chưa chết và còn HP
        hp -= amount; // Giảm HP
        ChangeState(TakeHit); // Chuyển sang trạng thái bị đánh
        hitTimer = 0.5f; // Đặt thời gian hiệu ứng
        if (hitSound.getStatus() != sf::Sound::Playing) hitSound.play(); // Phát âm thanh bị đánh
    }
}


void Enemy::ChangeState(EnemyState newState) {
    if (state == newState || !currentAnim) return;
    state = newState;
    switch (state) {
    case Idle: currentAnim = &idleAnim; break; 
    case Walk: currentAnim = &walkAnim; break;
    case Attack: currentAnim = &attackAnim; if (attackSound.getStatus() != sf::Sound::Playing) attackSound.play(); break;
    case TakeHit: currentAnim = &takeHitAnim; break;
    case Death: currentAnim = &deathAnim; break;
    }
    if (currentAnim && currentAnim->getTexture()) { 
        currentAnim->Reset();
        sprite.setTexture(*currentAnim->getTexture());
    }
}

// Cập nhật hitbox dựa trên sprite
void Enemy::updateHitbox() {
    if (currentAnim && currentAnim->getTexture()) {
        hitbox = sprite.getGlobalBounds();
        hitbox.width -= 50; 
        hitbox.height -= 50; 
        hitbox.left += 25; 
        hitbox.top += 25;
    }
}

// Tính khoảng cách đến player
float Enemy::getDistanceToPlayer(const sf::Vector2f& playerPos) {
    if (!currentAnim || !currentAnim->getTexture()) return 0.f;
    sf::Vector2f dx = playerPos - sprite.getPosition();
    return std::sqrt(dx.x * dx.x + dx.y * dx.y);
}

// Di chuyển về phía player
void Enemy::moveTowardsPlayer(float dt, const sf::Vector2f& playerPos) {
    if (!currentAnim || !currentAnim->getTexture()) return;
    ChangeState(Walk);
    sf::Vector2f dir = playerPos - sprite.getPosition();
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dist > 0) {
        dir /= dist;
        sprite.move(dir * speed * dt);
        facingLeft = dir.x < 0;
        sprite.setScale(facingLeft ? -0.5f : 0.5f, 0.5f);
    }
}

// Thực hiện tấn công
void Enemy::performAttack() {
    if (attackCooldown <= 0 && currentAnim && currentAnim->getTexture()) {
        ChangeState(Attack);
        attackCooldown = 2.0f;
    }
}

// Vẽ thanh HP
void Enemy::drawHPBar(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!currentAnim || !currentAnim->getTexture()) return; // Thoát nếu animation không hợp lệ
    sf::RectangleShape hpBar(sf::Vector2f(200.f, 15.f)); // Tạo thanh HP nền
    hpBar.setPosition(sprite.getPosition().x - 100.f, sprite.getPosition().y - 60.f); // Đặt vị trí
    hpBar.setFillColor(sf::Color::Black); // Màu đen cho nền
    target.draw(hpBar, states); // Vẽ nền

    sf::RectangleShape hpFill(sf::Vector2f(200.f * (static_cast<float>(hp) / maxHp), 15.f)); // Tạo thanh HP đầy
    hpFill.setPosition(sprite.getPosition().x - 100.f, sprite.getPosition().y - 60.f); // Đặt vị trí
    hpFill.setFillColor(sf::Color::Red); // Màu đỏ cho HP
    target.draw(hpFill, states); // Vẽ thanh HP
}
