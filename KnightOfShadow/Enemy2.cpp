#include "Enemy2.h"
#include <cmath>

Enemy2::Enemy2(sf::Texture & texIdle, sf::Texture & texWalk,
    sf::Texture & texAttack, sf::Texture & texDeath)
    : idleAnim(texIdle, { 6, 1 }, 0.2f),
    walkAnim(texWalk, { 10, 1 }, 0.1f),
    attackAnim(texAttack, { 14, 1 }, 0.1f, false),
    deathAnim(texDeath, { 16, 1 }, 0.15f, false)
{
    state = EnemyState::Idle;

    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400);
    sprite.setOrigin(rect.width / 2.f, rect.height / 2.f);
    sprite.setScale(0.3f, 0.3f);

    maxHealth = 500;   // ví dụ máu boss 2
    health = maxHealth;

    // Nền xám
    hpBack.setSize(sf::Vector2f(600.f, 20.f)); // dài 600, cao 20
    hpBack.setFillColor(sf::Color(50, 50, 50)); // màu xám
    hpBack.setPosition(500.f, 70.f);           // giữa màn hình

    // Thanh đỏ
    hpFront.setSize(sf::Vector2f(600.f, 20.f));
    hpFront.setFillColor(sf::Color::Red);
    hpFront.setPosition(hpBack.getPosition());
}

void Enemy2::HandleInput(float deltaTime, const sf::Vector2f& playerPosition)
{
    velocity.x = 0.f;

    if (state == EnemyState::Death || isDead)
        return;

    if (state == EnemyState::Attacking)
        return;

    sf::Vector2f direction = playerPosition - sprite.getPosition();
    float distance = std::hypot(direction.x, direction.y);
    facingRight = direction.x < 0;

    if (distance <= attackRange)
    {
        attackTimer += deltaTime;
        if (attackTimer >= attackCooldown)
        {
            attackTimer = 0.f;
            ChangeState(EnemyState::Attacking);
            hitboxActive = false;
        }
    }
    else if (distance <= detectionRange)
    {
        ChangeState(EnemyState::Walking);
        direction /= distance;
        velocity.x = direction.x * moveSpeed;
    }
    else
    {
        ChangeState(EnemyState::Idle);
    }

    sprite.setScale(facingRight ? 1.f : -1.f, 1.f);
}

void Enemy2::Update(float deltaTime)
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

    if (state != EnemyState::Attacking)
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
        UpdateAttackAnim(deltaTime, currentTexture, currentRect);
        break;

    case EnemyState::Death:
        deathAnim.Update(deltaTime);
        currentTexture = deathAnim.GetTexture();
        currentRect = deathAnim.GetRect();
        if (deathAnim.IsFinished())
            deathAnimFinished = true;
        break;
    }

    if (currentTexture && sprite.getTexture() != currentTexture)
        sprite.setTexture(*currentTexture);
    sprite.setTextureRect(currentRect);

    if (state == EnemyState::Attacking && hitboxActive)
    {
        sf::FloatRect attackBox;
        float enemyX = sprite.getPosition().x;
        float enemyY = sprite.getPosition().y;

        float offsetX = -140.f;

        attackBox.width = 150.f;
        attackBox.height = 200.f;
        attackBox.left = facingRight ? enemyX + offsetX : enemyX - offsetX - attackBox.width;
        attackBox.top = enemyY - 100.f;
        currentAttackBox = attackBox;
    }
    else
    {
        currentAttackBox = sf::FloatRect();
    }

    bodyHitbox.width = 150.f;
    bodyHitbox.height = 180.f;
    bodyOffset = sf::Vector2f(-bodyHitbox.width / 2.f, -bodyHitbox.height / 2.f);
    sf::Vector2f pos = sprite.getPosition();
    bodyHitbox.left = pos.x + bodyOffset.x;
    bodyHitbox.top = pos.y + bodyOffset.y;
}

void Enemy2::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);

    window.draw(hpBack);
    window.draw(hpFront);

    /*if (state == EnemyState::Attacking && hitboxActive)
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
    window.draw(bodyBox);*/
}

void Enemy2::UpdateAttackAnim(float deltaTime, const sf::Texture*& tex, sf::IntRect& rect)
{
    attackAnim.Update(deltaTime);
    tex = attackAnim.GetTexture();
    rect = attackAnim.GetRect();

    attackTimer += deltaTime;

    hitboxActive = (attackTimer >= attackActiveTime_Attack &&
        attackTimer <= attackActiveTime_Attack + attackDuration_Attack);

    if (attackAnim.IsFinished())
    {
        ChangeState(EnemyState::Idle);
        attackTimer = 0.f;
        hitboxActive = false;
    }
}

void Enemy2::ChangeState(EnemyState newState)
{
    if (state == newState || state == EnemyState::Death)
        return;

    state = newState;

    switch (state)
    {
    case EnemyState::Idle: idleAnim.Reset(); break;
    case EnemyState::Walking: walkAnim.Reset(); break;
    case EnemyState::Attacking: attackAnim.Reset(); attackTimer = 0.f; break;
    case EnemyState::Death: deathAnim.Reset(); break;
    }
}
void Enemy2::TakeDamage(int damage)
{
    BaseEnemy::TakeDamage(damage); // trừ máu

    // Cập nhật thanh đỏ
    float ratio = health / maxHealth;
    hpFront.setSize(sf::Vector2f(600.f * ratio, 20.f));
}