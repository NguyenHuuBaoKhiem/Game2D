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
    state = EnemyState::Idle;

    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400);
    sprite.setOrigin(rect.width / 2.f, rect.height / 2.f);
    sprite.setScale(1.f, 1.f);

    // Khởi tạo hitbox trống ban đầu
    currentAttackBox = sf::FloatRect();
    bodyHitbox = sf::FloatRect();
}

void Enemy::HandleInput(float deltaTime, const sf::Vector2f& playerPosition)
{
    velocity.x = 0.f;

    // Nếu boss đang tấn công hoặc trúng đòn hoặc chết => không điều khiển
    if (state == EnemyState::Attacking || state == EnemyState::TakeHit || state == EnemyState::Death)
    {
        if (attackCooldownTimer > 0)
            attackCooldownTimer -= deltaTime;
        return;
    }

    sf::Vector2f direction = playerPosition - sprite.getPosition();
    float distance = std::hypot(direction.x, direction.y);
    facingRight = direction.x < 0; // hướng về player

    if (distance <= attackRange && attackCooldownTimer <= 0.f)
    {
        ChangeState(EnemyState::Attacking);
        attackCooldownTimer = attackCooldown;
    }
    else if (distance <= detectionRange)
    {
        ChangeState(EnemyState::Walking);
        if (distance > 0.f)
        {
            direction /= distance;
            velocity.x = direction.x * moveSpeed;
        }
    }
    else
    {
        ChangeState(EnemyState::Idle);
    }

    sprite.setScale(facingRight ? 1.f : -1.f, 1.f);

    if (attackCooldownTimer > 0)
        attackCooldownTimer -= deltaTime;
}

void Enemy::Update(float deltaTime)
{
    if (state == EnemyState::Death && deathAnim.IsFinished())
        return;

    // Vật lý rơi cơ bản
    if (!isOnGround)
        velocity.y += gravity * deltaTime;

    sprite.move(velocity * deltaTime);

    if (sprite.getPosition().y >= groundY)
    {
        sprite.setPosition(sprite.getPosition().x, groundY);
        velocity.y = 0;
        isOnGround = true;
    }

    // Animation hiện tại
    const sf::Texture* currentTexture = nullptr;
    sf::IntRect currentRect;

    switch (state)
    {
    case EnemyState::Idle:
        idleAnim.Update(deltaTime);
        currentTexture = idleAnim.GetTexture();
        currentRect = idleAnim.GetRect();
        break;

    case EnemyState::Walking:
        walkAnim.Update(deltaTime);
        currentTexture = walkAnim.GetTexture();
        currentRect = walkAnim.GetRect();
        break;

    case EnemyState::Attacking:
        attackAnim.Update(deltaTime);
        currentTexture = attackAnim.GetTexture();
        currentRect = attackAnim.GetRect();

        attackTimer += deltaTime;

        // Bật/tắt hitbox theo thời gian
        if (attackTimer >= attackActiveTime && attackTimer <= attackActiveTime + attackDuration)
            hitboxActive = true;
        else
            hitboxActive = false;

        if (attackAnim.IsFinished())
        {
            ChangeState(EnemyState::Idle);
            attackTimer = 0.f;
            hitboxActive = false;
        }
        break;

    case EnemyState::TakeHit:
        takeHitAnim.Update(deltaTime);
        currentTexture = takeHitAnim.GetTexture();
        currentRect = takeHitAnim.GetRect();
        if (takeHitAnim.IsFinished())
            ChangeState(EnemyState::Idle);
        break;

    case EnemyState::Death:
        deathAnim.Update(deltaTime);
        currentTexture = deathAnim.GetTexture();
        currentRect = deathAnim.GetRect();
        break;
    }

    if (currentTexture && sprite.getTexture() != currentTexture)
        sprite.setTexture(*currentTexture);
    sprite.setTextureRect(currentRect);

    // --- HITBOX TẤN CÔNG ---
    if (state == EnemyState::Attacking && hitboxActive)
    {
        sf::FloatRect attackBox;
        attackBox.width = 250.f;
        attackBox.height = 250.f;
        float enemyX = sprite.getPosition().x;
        float enemyY = sprite.getPosition().y;
        attackBox.top = enemyY - 100.f;
        float offsetX = - 30.f;
        if (facingRight)
            attackBox.left = enemyX - (attackBox.width + offsetX);
        else
            attackBox.left = enemyX + offsetX;
        currentAttackBox = attackBox;
    }
    else
        currentAttackBox = sf::FloatRect();

    // --- HITBOX THÂN ---
    bodyHitbox.width = 200.f;
    bodyHitbox.height = 200.f;
    bodyOffset = sf::Vector2f(-bodyHitbox.width / 2.f, -bodyHitbox.height / 2.f);
    sf::Vector2f pos = sprite.getPosition();
    bodyHitbox.left = pos.x + bodyOffset.x;
    bodyHitbox.top = pos.y + (bodyOffset.y + 50.f);
}

void Enemy::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);

    // Hitbox tấn công (vàng)
    if (state == EnemyState::Attacking && hitboxActive)
    {
        sf::RectangleShape atkBoxShape;
        atkBoxShape.setPosition(currentAttackBox.left, currentAttackBox.top);
        atkBoxShape.setSize({ currentAttackBox.width, currentAttackBox.height });
        atkBoxShape.setFillColor(sf::Color(255, 255, 0, 60));
        atkBoxShape.setOutlineColor(sf::Color::Yellow);
        atkBoxShape.setOutlineThickness(1.f);
        window.draw(atkBoxShape);
    }

    // Hitbox thân (đỏ)
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
    if (state == newState || state == EnemyState::Death)
        return;

    state = newState;
    switch (state)
    {
    case EnemyState::Idle: idleAnim.Reset(); break;
    case EnemyState::Walking: walkAnim.Reset(); break;
    case EnemyState::Attacking: attackAnim.Reset(); break;
    case EnemyState::TakeHit: takeHitAnim.Reset(); break;
    case EnemyState::Death: deathAnim.Reset(); break;
    }
}

void Enemy::TakeDamage(int damage)
{
    if (state == EnemyState::Death) return;
    health -= damage;
    if (health <= 0)
        ChangeState(EnemyState::Death);
    else
        ChangeState(EnemyState::TakeHit);
}

bool Enemy::IsDead() const
{
    return state == EnemyState::Death && deathAnim.IsFinished();
}

// Getter / Setter
sf::Sprite& Enemy::GetSprite() { return sprite; }
sf::Vector2f Enemy::GetPosition() const { return sprite.getPosition(); }
void Enemy::SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
