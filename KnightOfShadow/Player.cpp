#include<iostream>
#include "Player.h"
#include "Animation.h"
#include<SFML/Window/Keyboard.hpp>

std::vector<DashTrail> dashTrails;
float trailSpawnTimer = 0.f;
float trailSpawnInterval = 0.025f;

Player::Player(sf::Texture& texIdle, sf::Texture& texWalk,
    sf::Texture& texAttack1, sf::Texture& texAttack2, sf::Texture& texAttack3)
    : idleAnim(texIdle, { 7, 1 }, 0.2f),
    walkAnim(texWalk, { 7, 1 }, 0.15f),
    attackAnim1(texAttack1, { 4, 1 }, 0.12f),
    attackAnim2(texAttack2, { 4, 1 }, 0.15f),
    attackAnim3(texAttack3, { 4, 1 }, 0.12f)
{
    // Load âm thanh 
    dashBuffer.loadFromFile("Assets/Sound effect/Player/dash.ogg");
    dashSound.setBuffer(dashBuffer);
    dashSound.setPitch(1.7f);
    dashSound.setVolume(7.f);

    jumpBuffer.loadFromFile("Assets/Sound effect/Player/jump.ogg");
    jumpSound.setBuffer(jumpBuffer);
    jumpSound.setVolume(60.f);

    at1_buffer.loadFromFile("Assets/Sound effect/Player/attack.ogg");
    at2_buffer.loadFromFile("Assets/Sound effect/Player/attack.ogg");
    at3_buffer.loadFromFile("Assets/Sound effect/Player/attack.ogg");

    // Bắt đầu với Idle
    state = Idle;

    // Texture ban đầu
    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());

    // Đặt origin theo frame đầu tiên để nhân vật đứng vững
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 400);
    sprite.setOrigin(384 / 2.f, 192 / 2.f);
    sprite.setScale(1.f, 1.f);
}

void Player::HandleInput(float deltaTime)
{
    velocity.x = 0.f;
    bool moving = false;

    float moveSpeed = 250.f;
    bool leftClick = sf::Keyboard::isKeyPressed(sf::Keyboard::X);

    if (dashCooldownTimer > 0)
        dashCooldownTimer -= deltaTime;

    // Nhấn Z để dash (nếu chưa dash và cooldown xong)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && !isDashing && dashCooldownTimer <= 0.f)
    {
        isDashing = true;
        dashTime = dashDuration;
        dashCooldownTimer = dashCooldown;
        dashSound.play();
    }
    // Chỉ cho di chuyển khi không tấn công
    if (state != Attacking1 && state != Attacking2 && state != Attacking3)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -moveSpeed;
            facingRight = false;
            moving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = +moveSpeed;
            facingRight = true;
            moving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && isOnGround)
        {
            velocity.y = jumpStrength;
            jumpSound.play();
            isOnGround = false;
        }
        // Flip sprite theo hướng
        sprite.setScale(facingRight ? 1.f : -1.f, 1.f);


        if (leftClick && !wasMousePressedLastFrame)
            ChangeState(Attacking1);
        else if (!leftClick)
            ChangeState(moving ? Walking : Idle);
    }
    else {
        // Nếu đang tấn công và người chơi nhấn click nữa → gắn combo
        if (leftClick && !wasMousePressedLastFrame) {
            comboQueued = true;
        }
    }

    wasMousePressedLastFrame = leftClick;
}

void Player::Update(float deltaTime)
{
    if (isDashing)
    {
        dashTime -= deltaTime;
        velocity.x = (facingRight ? dashSpeed : -dashSpeed);
        velocity.y = 0;
        sprite.move(velocity * deltaTime);

        trailSpawnTimer -= deltaTime;
        if (trailSpawnTimer <= 0.f)
        {
            DashTrail trail;
            trail.sprite = sprite; // copy sprite hiện tại
            trail.alpha = 200.f;   // độ trong suốt ban đầu (0–255)
            dashTrails.push_back(trail);
            trailSpawnTimer = trailSpawnInterval;
        }

        if (dashTime <= 0)
        {
            isDashing = false;
            velocity.x = 0;
            ChangeState(Idle);
        }
    }
    for (auto& t : dashTrails)
    {
        t.alpha -= 1000.f * deltaTime;
        if (t.alpha < 0) t.alpha = 0;
        t.sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(t.alpha)));
    }

    dashTrails.erase(std::remove_if(dashTrails.begin(), dashTrails.end(),
        [](const DashTrail& t) { return t.alpha <= 0; }), dashTrails.end());

    if (!isOnGround) {
        velocity.y += gravity * deltaTime;
    }
    sprite.move(velocity * deltaTime);

    // Kiểm tra chạm đất
    if (sprite.getPosition().y >= groundY) {
        sprite.setPosition(sprite.getPosition().x, groundY);
        velocity.y = 0;
        isOnGround = true;
    }

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
    case Attacking1:
        attackAnim1.Update(deltaTime, false);
        currentTexture = attackAnim1.getTexture();
        currentRect = attackAnim1.GetRect();
        if (attackAnim1.IsFinished()) {
            if (comboQueued) {
                comboQueued = false;
                ChangeState(Attacking2);
            }
            else {
                ChangeState(Idle);
            }
        }
        break;

    case Attacking2:
        attackAnim2.Update(deltaTime, false);
        currentTexture = attackAnim2.getTexture();
        currentRect = attackAnim2.GetRect();
        if (attackAnim2.IsFinished()) {
            if (comboQueued) {
                comboQueued = false;
                ChangeState(Attacking3);
            }
            else {
                ChangeState(Idle);
            }
        }
        break;

    case Attacking3:
        attackAnim3.Update(deltaTime, false);
        currentTexture = attackAnim3.getTexture();
        currentRect = attackAnim3.GetRect();
        if (attackAnim3.IsFinished()) {
            comboQueued = false;
            ChangeState(Idle);
        }
        break;
    }
    if (currentTexture && sprite.getTexture() != currentTexture)
        sprite.setTexture(*currentTexture);

    sprite.setTextureRect(currentRect);

}

void Player::Draw(sf::RenderWindow& window)
{
    for (const auto& t : dashTrails)
        window.draw(t.sprite);
    window.draw(sprite);
}

void Player::ChangeState(PlayerState newState)
{
    if (state == newState) return;

    state = newState;

    switch (state)
    {
    case Idle:       idleAnim.Reset();      break;
    case Walking:    walkAnim.Reset();      break;
    case Attacking1:
        attackAnim1.Reset();
        attackSound.setBuffer(at1_buffer);
        attackSound.setPitch(0.95f + (rand() % 5 - 2) / 100.f);
        attackSound.setVolume(40.f);
        attackSound.play();
        {
            sf::Sound temp = attackSound;
            sf::sleep(sf::milliseconds(15));
            temp.play();
        }
        break;
    case Attacking2:
        attackAnim2.Reset();
        attackSound.setBuffer(at2_buffer);
        attackSound.setPitch(1.00f + (rand() % 5 - 2) / 100.f);
        attackSound.setVolume(25.f);
        attackSound.play();
        {
            sf::Sound temp = attackSound;
            sf::sleep(sf::milliseconds(10)); // lưu ý: trong game thực tế nên dùng timer
            temp.play();
        }
        break;
    case Attacking3:
        attackAnim3.Reset();
        attackSound.setBuffer(at3_buffer);
        attackSound.setPitch(1.05f + (rand() % 5 - 2) / 100.f); // ±2%
        attackSound.setVolume(15.f);
        attackSound.play();
        break;
    }
}

// ----------------- Getter / Setter -----------------
sf::Sprite& Player::GetSprite() { return sprite; }
sf::Vector2f Player::GetPosition() const { return sprite.getPosition(); }
void Player::SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
