#include "Enemy.h"
#include <cmath>

Enemy::Enemy(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack,
    sf::Texture& texTakeHit, sf::Texture& texDeath)
    : idleAnim(texIdle, { 6, 1 }, 0.2f), // 6 frame, mỗi frame 0.2s
    walkAnim(texWalk, { 12, 1 }, 0.1f), // 12 frame, mỗi frame 0.1s
    attackAnim(texAttack, { 15, 1 }, 0.08f, false), // 15 frame, không lặp
    takeHitAnim(texTakeHit, { 5, 1 }, 0.1f, false), // 5 frame, không lặp
    deathAnim(texDeath, { 22, 1 }, 0.1f, false) // 22 frame, không lặp
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

    // Bắt đầu với trạng thái Idle
    state = Idle;

    // Texture ban đầu
    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());

    // Đặt origin và vị trí ban đầu
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400); // Vị trí ban đầu (có thể thay đổi)
    sprite.setOrigin(rect.width / 2.f, rect.height / 2.f);
    sprite.setScale(1.f, 1.f);
}

void Enemy::HandleInput(float deltaTime, const sf::Vector2f& playerPosition)
{
    // Reset vận tốc ngang
    velocity.x = 0.f;

    // Không xử lý input nếu đang attacking, takehit, hoặc death
    if (state != Attacking && state != TakeHit && state != Death)
    {
        // Tính hướng và vị trí tương đối đến player
        sf::Vector2f direction = playerPosition - sprite.getPosition();
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // Xác định hướng mặt (đảo ngược logic để khớp sprite sheet)
        facingRight = direction.x < 0; // Nếu player bên trái, quay mặt phải

        // Tạo vùng nhận diện hình chữ nhật, bắt đầu từ xa enemy và mở rộng
        sf::FloatRect detectionArea;
        detectionArea.left = sprite.getPosition().x - 400.f; // Bắt đầu cách 400px sang trái
        detectionArea.top = sprite.getPosition().y - 200.f;  // Bắt đầu cách 200px lên trên
        detectionArea.width = 800.f;  // Tổng chiều ngang 800px (400px mỗi bên)
        detectionArea.height = 600.f; // Tổng chiều dọc 600px (200px lên trên, 400px xuống dưới)

        // Kiểm tra xem player có trong vùng nhận diện không
        if (detectionArea.contains(playerPosition))
        {
            if (distance <= attackRange && attackCooldownTimer <= 0.f)
            {
                ChangeState(Attacking);
                attackCooldownTimer = attackCooldown;
            }
            else if (distance <= detectionRange)
            {
                ChangeState(Walking);
                if (distance > 0)
                {
                    direction /= distance; // Chuẩn hóa vector hướng
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
            ChangeState(Idle); // Nếu ngoài vùng nhận diện, đứng yên
        }

        // Flip sprite theo hướng
        sprite.setScale(facingRight ? 1.f : -1.f, 1.f); // 1.f: mặt hướng trái, -1.f: mặt hướng phải
    }

    // Giảm thời gian hồi chiêu tấn công
    if (attackCooldownTimer > 0)
        attackCooldownTimer -= deltaTime;
}

void Enemy::Update(float deltaTime)
{
    // Không update nếu đã chết
    if (state == Death && deathAnim.IsFinished())
        return;

    // Xử lý vật lý (tương tự player)
    if (!isOnGround)
    {
        velocity.y += gravity * deltaTime;
    }
    sprite.move(velocity * deltaTime);

    // Kiểm tra chạm đất
    if (sprite.getPosition().y >= groundY)
    {
        sprite.setPosition(sprite.getPosition().x, groundY);
        velocity.y = 0;
        isOnGround = true;
    }

    // Cập nhật animation theo trạng thái
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
        {
            ChangeState(Idle);
        }
        break;

    case TakeHit:
        takeHitAnim.Update(deltaTime, false);
        currentTexture = takeHitAnim.getTexture();
        currentRect = takeHitAnim.GetRect();
        if (takeHitAnim.IsFinished())
        {
            ChangeState(Idle);
        }
        break;

    case Death:
        deathAnim.Update(deltaTime, false);
        currentTexture = deathAnim.getTexture();
        currentRect = deathAnim.GetRect();
        break;
    }

    // Cập nhật texture và frame
    if (currentTexture && sprite.getTexture() != currentTexture)
        sprite.setTexture(*currentTexture);
    sprite.setTextureRect(currentRect);

    // Cập nhật hitbox tấn công (xa enemy hơn và dài xuống dưới chân)
    if (state == Attacking)
    {
        sf::FloatRect attackBox;
        attackBox.width = 500.f;  // Chiều rộng 200px
        attackBox.height = 700.f; // Tăng chiều cao lên 300px để dài xuống dưới

        float enemyX = sprite.getPosition().x;
        float enemyY = sprite.getPosition().y;
        attackBox.top = enemyY + 200.f; // Hạ thấp hitbox, bắt đầu từ dưới chân (giả sử +50px từ tâm)

        float offsetX = 300.f; // Tăng khoảng cách từ giữa enemy ra phía trước lên 150px, xa hơn
        if (facingRight)
            attackBox.left = enemyX - (attackBox.width + offsetX); // Mặt hướng trái, hitbox bên trái
        else
            attackBox.left = enemyX + offsetX; // Mặt hướng phải, hitbox bên phải

        currentAttackBox = attackBox;
    }
    else
    {
        currentAttackBox = sf::FloatRect();
    }

    // Cập nhật hitbox thân (tăng dài và rộng ra)
    bodyHitbox.width = 300.f;  // Tăng chiều ngang lên 150px
    bodyHitbox.height = 500.f; // Tăng chiều cao lên 200px
    bodyOffset = sf::Vector2f(-bodyHitbox.width / 2.f, -bodyHitbox.height + 100.f); // Hạ thấp hơn, cách chân 40px
    sf::Vector2f pos = sprite.getPosition();
    bodyHitbox.left = pos.x + bodyOffset.x;
    bodyHitbox.top = pos.y + bodyOffset.y;
}

void Enemy::Draw(sf::RenderWindow& window)
{
    window.draw(sprite);

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

    // Vẽ vùng nhận diện để debug
    sf::RectangleShape detectionBox;
    detectionBox.setPosition(detectionArea.left, detectionArea.top);
    detectionBox.setSize({ detectionArea.width, detectionArea.height });
    detectionBox.setFillColor(sf::Color(0, 255, 0, 30)); // Xanh nhạt trong suốt
    detectionBox.setOutlineColor(sf::Color::Green);
    detectionBox.setOutlineThickness(1.f);
    window.draw(detectionBox);

    // Vẽ vùng thân
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
    case Idle:
        idleAnim.Reset();
        break;
    case Walking:
        walkAnim.Reset();
        break;
    case Attacking:
        attackAnim.Reset();
        //attackSound.setPitch(0.95f + (rand() % 5 - 2) / 100.f);
        //attackSound.play();
        break;
    case TakeHit:
        takeHitAnim.Reset();
        //takeHitSound.play();
        break;
    case Death:
        deathAnim.Reset();
        //deathSound.play();
        break;
    }
}

void Enemy::TakeDamage(int damage)
{
    if (state == Death)
        return;

    health -= damage;
    if (health <= 0)
    {
        ChangeState(Death);
    }
    else
    {
        ChangeState(TakeHit);
    }
}

bool Enemy::IsDead() const
{
    return state == Death && deathAnim.IsFinished();
}

// Getter / Setter
sf::Sprite& Enemy::GetSprite() { return sprite; }
sf::Vector2f Enemy::GetPosition() const { return sprite.getPosition(); }
void Enemy::SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
