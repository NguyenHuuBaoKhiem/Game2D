#include "Enemy.h"
#include <cmath>
#include <iostream>

Enemy::Enemy(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack,
    sf::Texture& texTakeHit, sf::Texture& texDeath)
    : idleAnim(texIdle, { 6, 1 }, 0.2f),
    walkAnim(texWalk, { 12, 1 }, 0.1f),
    attackAnim(texAttack, { 15, 1 }, 0.08f, false),
    takeHitAnim(texTakeHit, { 5, 1 }, 0.1f, false),
    deathAnim(texDeath, { 22, 1 }, 0.1f, false)
{

    // Load âm thanh (commented out, giữ nguyên)
    //attackBuffer.loadFromFile("Assets/Sound effect/Enemy/attack.ogg");
    //attackSound.setBuffer(attackBuffer);
    //attackSound.setVolume(25.f);

    //takeHitBuffer.loadFromFile("Assets/Sound effect/Enemy/takehit.ogg");
    //takeHitSound.setBuffer(takeHitBuffer);
    //takeHitSound.setVolume(30.f);

    //deathBuffer.loadFromFile("Assets/Sound effect/Enemy/death.ogg");
    //deathSound.setBuffer(deathBuffer);
    //deathSound.setVolume(40.f);

    state = Idle;
    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400);
    sprite.setOrigin(rect.width / 2.f, rect.height / 2.f);
    sprite.setScale(1.f, 1.f);
}

void Enemy::HandleInput(float deltaTime, const sf::Vector2f& playerPosition)
{
    velocity.x = 0.f;

    if (state == Attacking || state == TakeHit || state == Death)
    {
        if (attackCooldownTimer > 0)
            attackCooldownTimer -= deltaTime;
        return;
    }

    sf::Vector2f direction = playerPosition - sprite.getPosition();
    float distance = std::hypot(direction.x, direction.y);
    facingRight = direction.x < 0;

    // DÙNG detectionArea ĐÃ ĐƯỢC SET Ở Update() → FULL MAP
    if (detectionArea.contains(playerPosition))
    {
        if (distance <= attackRange && attackCooldownTimer <= 0.f)
        {
            ChangeState(Attacking);
            attackCooldownTimer = attackCooldown;
        }
        else if (distance <= detectionRange)  // DÍ TỪ XA (2000px)
        {
            ChangeState(Walking);
            if (distance > 0.f)
            {
                direction /= distance;
                velocity.x = direction.x * moveSpeed;
            }
        }
        else
        {
            ChangeState(Idle);
        }
    }
    else
    {
        ChangeState(Idle);
    }

    sprite.setScale(facingRight ? 1.f : -1.f, 1.f);

    if (attackCooldownTimer > 0)
        attackCooldownTimer -= deltaTime;
}

void Enemy::Update(float deltaTime)
{
    if (state == Death && deathAnim.IsFinished())
        return;

    // CẬP NHẬT VÙNG PHÁT HIỆN: FULL MAP 1600x894
    detectionArea = sf::FloatRect(0.f, 0.f, 1600.f, 894.f);

    // Vật lý
    if (!isOnGround)
        velocity.y += gravity * deltaTime;

    sprite.move(velocity * deltaTime);

    if (sprite.getPosition().y >= groundY)
    {
        sprite.setPosition(sprite.getPosition().x, groundY);
        velocity.y = 0;
        isOnGround = true;
    }

    // Animation
    const sf::Texture* currentTexture = nullptr;
    sf::IntRect currentRect;

    switch (state)
    {
    case Idle:
        idleAnim.Update(deltaTime, true);
        currentTexture = idleAnim.getTexture();
        currentRect = idleAnim.GetRect();
        break;
    case Walking:
        walkAnim.Update(deltaTime, true);
        currentTexture = walkAnim.getTexture();
        currentRect = walkAnim.GetRect();
        break;
    case Attacking:
        attackAnim.Update(deltaTime, false);
        currentTexture = attackAnim.getTexture();
        currentRect = attackAnim.GetRect();
        if (attackAnim.IsFinished())
            ChangeState(Idle);
        break;
    case TakeHit:
        takeHitAnim.Update(deltaTime, false);
        currentTexture = takeHitAnim.getTexture();
        currentRect = takeHitAnim.GetRect();
        if (takeHitAnim.IsFinished())
            ChangeState(Idle);
        break;
    case Death:
        deathAnim.Update(deltaTime, false);
        currentTexture = deathAnim.getTexture();
        currentRect = deathAnim.GetRect();
        break;
    }

    if (currentTexture && sprite.getTexture() != currentTexture)
        sprite.setTexture(*currentTexture);
    sprite.setTextureRect(currentRect);

    // HITBOX TẤN CÔNG (VÀNG) – VỪA ĐỦ
    if (state == Attacking)
    {
        sf::FloatRect attackBox;
        attackBox.width = 250.f;
        attackBox.height = 280.f;
        float enemyX = sprite.getPosition().x;
        float enemyY = sprite.getPosition().y;
        attackBox.top = enemyY - 140.f;
        float offsetX = 100.f;

        if (facingRight)
            attackBox.left = enemyX - (attackBox.width + offsetX);
        else
            attackBox.left = enemyX + offsetX;

        currentAttackBox = attackBox;

    }
    else
    {
        currentAttackBox = sf::FloatRect();
    }

    // HITBOX THÂN (ĐỎ) – CĂN GIỮA
    bodyHitbox.width = 300.f;
    bodyHitbox.height = 300.f;
    bodyOffset = sf::Vector2f(
        -bodyHitbox.width / 2.f,
        -bodyHitbox.height / 2.f
    );
    sf::Vector2f pos = sprite.getPosition();
    bodyHitbox.left = pos.x + bodyOffset.x;
    bodyHitbox.top = pos.y + bodyOffset.y;

}

void Enemy::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);

    // Vẽ hitbox tấn công (vàng)
    if (state == Attacking)
    {
        sf::RectangleShape atkBoxShape;
        atkBoxShape.setPosition(currentAttackBox.left, currentAttackBox.top);
        atkBoxShape.setSize({ currentAttackBox.width, currentAttackBox.height });
        atkBoxShape.setFillColor(sf::Color(255, 255, 0, 60));
        atkBoxShape.setOutlineColor(sf::Color::Yellow);
        atkBoxShape.setOutlineThickness(1.f);
        window.draw(atkBoxShape);
    }

    // Vẽ vùng phát hiện (xanh) – FULL MAP
    sf::RectangleShape detectionBox;
    detectionBox.setPosition(detectionArea.left, detectionArea.top);
    detectionBox.setSize({ detectionArea.width, detectionArea.height });
    detectionBox.setFillColor(sf::Color(0, 255, 0, 30));
    detectionBox.setOutlineColor(sf::Color::Green);
    detectionBox.setOutlineThickness(1.f);
    window.draw(detectionBox);

    // Vẽ hitbox thân (đỏ)
    sf::RectangleShape bodyBox;
    bodyBox.setPosition(bodyHitbox.left, bodyHitbox.top);
    bodyBox.setSize({ bodyHitbox.width, bodyHitbox.height });
    bodyBox.setFillColor(sf::Color(255, 0, 0, 40));
    bodyBox.setOutlineColor(sf::Color::Red);
    bodyBox.setOutlineThickness(1.f);
    window.draw(bodyBox);
}

void Enemy::ChangeState(EnemyState newState)
{
    if (state == newState || state == Death)
        return;

    state = newState;
    switch (state)
    {
    case Idle:     idleAnim.Reset(); break;
    case Walking:  walkAnim.Reset(); break;
    case Attacking: attackAnim.Reset(); break;
    case TakeHit:  takeHitAnim.Reset(); break;
    case Death:    deathAnim.Reset(); break;
    }
}

void Enemy::TakeDamage(int damage)
{
    if (state == Death) return;
    health -= damage;
    if (health <= 0)
        ChangeState(Death);
    else
        ChangeState(TakeHit);
}

bool Enemy::IsDead() const
{
    return state == Death && deathAnim.IsFinished();
}

// Getter / Setter
sf::Sprite& Enemy::GetSprite() { return sprite; }
sf::Vector2f Enemy::GetPosition() const { return sprite.getPosition(); }
void Enemy::SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
