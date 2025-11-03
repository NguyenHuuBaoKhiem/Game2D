#include "Enemy1.h"
#include <cmath>
#include <iostream>

Enemy1::Enemy1(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack,sf::Texture& texDeath)
    : idleAnim(texIdle, { 6, 1 }, 0.2f),
    walkAnim(texWalk, { 12, 1 }, 0.1f),
    attackAnim(texAttack, { 15, 1 }, 0.08f, false),
    deathAnim(texDeath, { 10, 2 }, 0.15f, false)
{
    state = EnemyState::Idle;

    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400);
    sprite.setOrigin(rect.width / 2.f, rect.height / 2.f);
    sprite.setScale(1.f, 1.f);

    currentAttackBox = sf::FloatRect();
    bodyHitbox = sf::FloatRect();
}

void Enemy1::HandleInput(float deltaTime, const sf::Vector2f& playerPosition)
{
    velocity.x = 0.f;

    if (state == EnemyState::Attacking || state == EnemyState::Death || isDead)
    {
        if (attackCooldownTimer > 0)
            attackCooldownTimer -= deltaTime;
        return;
    }

    sf::Vector2f direction = playerPosition - sprite.getPosition();
    float distance = std::hypot(direction.x, direction.y);
    facingRight = direction.x < 0;

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

void Enemy1::Update(float deltaTime)
{
    UpdateHitCooldown(deltaTime);

    if (isDead)
    {
        if (state != EnemyState::Death)
            ChangeState(EnemyState::Death);
    }

    if (state == EnemyState::Death && deathAnim.IsFinished())
        return;

    if (!isOnGround)
        velocity.y += gravity * deltaTime;

    sprite.move(velocity * deltaTime);

    if (sprite.getPosition().y >= groundY)
    {
        sprite.setPosition(sprite.getPosition().x, groundY);
        velocity.y = 0;
        isOnGround = true;
    }

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

    case EnemyState::Death:
        deathAnim.Update(deltaTime);
        currentTexture = deathAnim.GetTexture();
        currentRect = deathAnim.GetRect();

        // Nếu animation chết đã chạy hết → đánh dấu hoàn tất
        if (deathAnim.IsFinished()) {
            deathAnimFinished = true; // <--- biến có sẵn trong BaseEnemy
        }
        break;
    }

    if (currentTexture && sprite.getTexture() != currentTexture)
        sprite.setTexture(*currentTexture);
    sprite.setTextureRect(currentRect);

    if (state == EnemyState::Attacking && hitboxActive)
    {
        sf::FloatRect attackBox;
        attackBox.width = 250.f;
        attackBox.height = 250.f;
        float enemyX = sprite.getPosition().x;
        float enemyY = sprite.getPosition().y;
        attackBox.top = enemyY - 100.f;
        float offsetX = -30.f;
        if (facingRight)
            attackBox.left = enemyX - (attackBox.width + offsetX);
        else
            attackBox.left = enemyX + offsetX;
        currentAttackBox = attackBox;
    }
    else
        currentAttackBox = sf::FloatRect();

    bodyHitbox.width = 200.f;
    bodyHitbox.height = 200.f;
    bodyOffset = sf::Vector2f(-bodyHitbox.width / 2.f, -bodyHitbox.height / 2.f);
    sf::Vector2f pos = sprite.getPosition();
    bodyHitbox.left = pos.x + bodyOffset.x;
    bodyHitbox.top = pos.y + (bodyOffset.y + 50.f);
}

void Enemy1::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);

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

    sf::RectangleShape bodyBox;
    bodyBox.setPosition(bodyHitbox.left, bodyHitbox.top);
    bodyBox.setSize({ bodyHitbox.width, bodyHitbox.height });
    bodyBox.setFillColor(sf::Color(255, 0, 0, 40));
    bodyBox.setOutlineColor(sf::Color::Red);
    bodyBox.setOutlineThickness(1.f);
    window.draw(bodyBox);
}

void Enemy1::ChangeState(EnemyState newState)
{
    if (state == newState || state == EnemyState::Death)
        return;

    state = newState;
    switch (state)
    {
    case EnemyState::Idle: idleAnim.Reset(); break;
    case EnemyState::Walking: walkAnim.Reset(); break;
    case EnemyState::Attacking: attackAnim.Reset(); break;
    case EnemyState::Death: deathAnim.Reset(); break;
    }
}

void Enemy1::TakeDamage(int damage)
{
    BaseEnemy::TakeDamage(damage);
}
