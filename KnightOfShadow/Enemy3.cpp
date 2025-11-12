#include "Enemy3.h"
#include <cmath>
#include <iostream>

Enemy3::Enemy3(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack,
    sf::Texture& texAttack1, sf::Texture& texAttack2, sf::Texture& texTele, sf::Texture& texDeath)
    : idleAnim(texIdle, { 9, 1 }, 0.15f),
    walkAnim(texWalk, { 8, 1 }, 0.1f),
    attackAnim(texAttack, { 10, 4 }, 0.1f, false),
    attack1Anim(texAttack1, { 10, 2 }, 0.1f, false),
    attack2Anim(texAttack2, { 7, 7 }, 0.1f, false),
    teleAnim(texTele, { 10, 1 }, 0.1f, false),
    deathAnim(texDeath, { 7, 6 }, 0.15f, false)
{
    state = EnemyState::Idle;
    attackType = Boss3AttackType::None;

    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400);
    sprite.setOrigin(rect.width / 2.f, rect.height / 2.f);
    sprite.setScale(0.7f, 0.7f);

    currentAttackBox = sf::FloatRect();
    bodyHitbox = sf::FloatRect();
}

void Enemy3::HandleInput(float deltaTime, const sf::Vector2f& playerPosition)
{
    velocity.x = 0.f;

    // ==== Nếu chuẩn bị Attack2 thì teleport lên đầu player ====
    if (isTeleportForAttack2)
    {
        isTeleporting = true;
        teleTarget = attack2Target;
        isTeleportForAttack2 = false; // reset
        teleAnim.Reset();
        teleTimer = 0.f;
        return; // ngừng mọi logic khác cho tới khi teleport xong
    }


    // Nếu đang tấn công hoặc chết thì không di chuyển
    if (state == EnemyState::Attacking || state == EnemyState::Death || isDead)
    {
        if (attackCooldownTimer > 0)
            attackCooldownTimer -= deltaTime;
        return;
    }

    // Nếu đang teleport thì dừng hết
    if (isTeleporting)
        return;

    sf::Vector2f direction = playerPosition - sprite.getPosition();
    float distance = std::hypot(direction.x, direction.y);
    facingRight = direction.x > 0;

    // Kiểm tra cooldown teleport
    teleTimer += deltaTime;
    if (teleTimer >= teleCooldown && !isTeleporting && state != EnemyState::Attacking)
    {
        isTeleporting = true;
        teleTimer = 0.f;
        float offsetX = (rand() % 2 == 0 ? -150.f : 150.f);
        teleTarget = sf::Vector2f(playerPosition.x + offsetX, playerPosition.y);
        teleAnim.Reset();
        return;
    }

    // Kiểm tra tấn công
    if (distance <= attackRange)
    {
        attackTimer += deltaTime;

        if (attackTimer >= attackCooldown)
        {
            attackTimer = 0.f;
            int r = rand() % 100; // số từ 0 -> 99
            if (r < 50)            // 0-49 -> 50%
            {
                attackType = Boss3AttackType::Attack2;
                isTeleportForAttack2 = true;
                attack2Target = sf::Vector2f(playerPosition.x, playerPosition.y - 250.f);
            }
            else if (r < 80)       // 50-79 -> 30%
                attackType = Boss3AttackType::Attack1;
            else                   // 80-99 -> 20%
                attackType = Boss3AttackType::Attack;
         


            ChangeState(EnemyState::Attacking);
            hitboxActive = false;
            attackCooldownTimer = 0.f;
        }
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

    sprite.setScale(facingRight ? 0.7f : -0.7f, 0.7f);

    if (attackCooldownTimer > 0)
        attackCooldownTimer -= deltaTime;
}

void Enemy3::Update(float deltaTime)
{
    UpdateHitCooldown(deltaTime);

    // ===== TELEPORT CHO ATTACK2 =====
    if (isTeleportForAttack2)
    {
        teleAnim.Reset();
        isTeleporting = true;      // dùng lại cơ chế teleport animation
        teleTarget = attack2Target;
        isTeleportForAttack2 = false;  // reset flag để không lặp lại
        teleTimer = 0.f;
        return; // ngừng các hành vi khác khi teleport
    }

    if (isDead)
    {
        if (state != EnemyState::Death)
            ChangeState(EnemyState::Death);
    }

    if (isTeleporting)
    {
        teleAnim.Update(deltaTime);
        sprite.setTexture(*teleAnim.GetTexture());
        sprite.setTextureRect(teleAnim.GetRect());
        sprite.setScale(facingRight ? 0.7f : -0.7f, 0.7f);

        if (teleAnim.IsFinished())
        {
            sprite.setPosition(teleTarget);
            isTeleporting = false;

            // Nếu teleport chuẩn bị Attack2
            if (attackType == Boss3AttackType::Attack2)
            {
                ChangeState(EnemyState::Attacking);
                attackTimer = 0.f;
                hitboxActive = false;
            }
            else
            {
                // Thực hiện teleport bình thường (attack ngẫu nhiên)
                attackType = (rand() % 2 == 0) ? Boss3AttackType::Attack : Boss3AttackType::Attack1;
                ChangeState(EnemyState::Attacking);
                attackTimer = 0.f;
                hitboxActive = false;
            }
        }
        return;
    }



    if (!isOnGround)
        velocity.y += gravity * deltaTime;
    // Dừng mọi di chuyển khi chuẩn bị hoặc đang thực hiện Attack2
    if (isTeleportForAttack2 || (attackType == Boss3AttackType::Attack2 && state == EnemyState::Attacking))
    {
        velocity = sf::Vector2f(0.f, 0.f); // dừng mọi di chuyển
    }

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
    sprite.setScale(facingRight ? 0.7f : -0.7f, 0.7f);

    // Hitbox tấn công
    if (state == EnemyState::Attacking && hitboxActive)
    {
        sf::FloatRect attackBox;
        float enemyX = sprite.getPosition().x;
        float enemyY = sprite.getPosition().y;

        switch (attackType)
        {
        case Boss3AttackType::Attack:
            attackBox.width = 250.f; attackBox.height = 250.f;
            attackBox.top = enemyY - 100.f;
            attackBox.left = facingRight ? enemyX - 220.f : enemyX + 20.f;
            break;
        case Boss3AttackType::Attack1:
            attackBox.width = 500.f; attackBox.height = 50.f;
            attackBox.top = enemyY + 130.f;
            attackBox.left = enemyX - attackBox.width / 2.f;
            break;
        case Boss3AttackType::Attack2:
            attackBox.width = 300.f; attackBox.height = 400.f;
            attackBox.top = enemyY - 200.f;
            attackBox.left = facingRight ? enemyX - 300.f : enemyX + 20.f;
            break;
        default:
            attackBox = sf::FloatRect();
            break;
        }

        currentAttackBox = attackBox;
    }
    else
        currentAttackBox = sf::FloatRect();

    // Hitbox thân
    bodyHitbox.width = 200.f;
    bodyHitbox.height = 200.f;
    bodyOffset = sf::Vector2f(-bodyHitbox.width / 2.f, -bodyHitbox.height / 2.f);
    sf::Vector2f pos = sprite.getPosition();
    bodyHitbox.left = pos.x + bodyOffset.x;
    bodyHitbox.top = pos.y + (bodyOffset.y + 50.f);
}


void Enemy3::UpdateAttackAnim(float deltaTime, const sf::Texture*& tex, sf::IntRect& rect)
{
    attackTimer += deltaTime;

    switch (attackType)
    {
    case Boss3AttackType::Attack:
        attackAnim.Update(deltaTime);
        tex = attackAnim.GetTexture();
        rect = attackAnim.GetRect();
        break;
    case Boss3AttackType::Attack1:
        attack1Anim.Update(deltaTime);
        tex = attack1Anim.GetTexture();
        rect = attack1Anim.GetRect();
        break;
    case Boss3AttackType::Attack2:
        attack2Anim.Update(deltaTime);
        tex = attack2Anim.GetTexture();
        rect = attack2Anim.GetRect();
        break;
    default:
        tex = idleAnim.GetTexture();
        rect = idleAnim.GetRect();
        break;
    }

    if (attackType == Boss3AttackType::Attack)
    {
        float moveSpeedWhileAttacking = 250.f; // tốc độ di chuyển khi đánh
        sprite.move((facingRight ? 0.7f : -0.7f) * moveSpeedWhileAttacking * deltaTime, 0.f);
    }

    switch (attackType)
    {
    case Boss3AttackType::Attack:
        hitboxActive = (attackTimer >= attackActiveTime_Attack &&
            attackTimer <= attackActiveTime_Attack + attackDuration_Attack);
        break;

    case Boss3AttackType::Attack1:
        hitboxActive = (attackTimer >= attackActiveTime_Attack1 &&
            attackTimer <= attackActiveTime_Attack1 + attackDuration_Attack1);
        break;

    case Boss3AttackType::Attack2:
        hitboxActive = (attackTimer >= attackActiveTime_Attack2 &&
            attackTimer <= attackActiveTime_Attack2 + attackDuration_Attack2);
        break;

    default:
        hitboxActive = false;
        break;
    }

    bool finished = false;
    if (attackType == Boss3AttackType::Attack) finished = attackAnim.IsFinished();
    if (attackType == Boss3AttackType::Attack1) finished = attack1Anim.IsFinished();
    if (attackType == Boss3AttackType::Attack2) finished = attack2Anim.IsFinished();

    if (finished)
    {
        ChangeState(EnemyState::Idle);
        attackType = Boss3AttackType::None;
        attackTimer = 0.f;
        hitboxActive = false;
    }
}


void Enemy3::Draw(sf::RenderWindow& window)
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

    // Hitbox thân
    sf::RectangleShape bodyBox;
    bodyBox.setPosition(bodyHitbox.left, bodyHitbox.top);
    bodyBox.setSize({ bodyHitbox.width, bodyHitbox.height });
    bodyBox.setFillColor(sf::Color(255, 0, 0, 40));
    bodyBox.setOutlineColor(sf::Color::Red);
    bodyBox.setOutlineThickness(1.f);
    window.draw(bodyBox);
}

void Enemy3::ChangeState(EnemyState newState)
{
    if (state == newState || state == EnemyState::Death)
        return;

    state = newState;

    switch (state)
    {
    case EnemyState::Idle: idleAnim.Reset(); break;
    case EnemyState::Walking: walkAnim.Reset(); break;
    case EnemyState::Attacking:
        if (attackType == Boss3AttackType::Attack) attackAnim.Reset();
        else if (attackType == Boss3AttackType::Attack1) attack1Anim.Reset();
        else if (attackType == Boss3AttackType::Attack2) attack2Anim.Reset();
        break;
    case EnemyState::Death: deathAnim.Reset();
        isTeleporting = false;
        deathAnim.Reset();
        break;
    }
}

void Enemy3::TakeDamage(int damage)
{
    BaseEnemy::TakeDamage(damage);
}
