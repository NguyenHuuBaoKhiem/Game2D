#include "Player.h"
#include "Animation.h"
#include<SFML/Window/Keyboard.hpp>
#include<SFML/Window/Mouse.hpp>

Player::Player(sf::Texture& texIdle, sf::Texture& texWalk,
    sf::Texture& texAttack1, sf::Texture& texAttack2, sf::Texture& texAttack3)
    : idleAnim(texIdle, { 7, 1 }, 0.2f),
    walkAnim(texWalk, { 7, 1 }, 0.15f),
    attackAnim1(texAttack1, { 4, 1 }, 0.2f),
    attackAnim2(texAttack2, { 4, 1 }, 0.2f),
    attackAnim3(texAttack3, { 4, 1 }, 0.2f)
{
    // Bắt đầu với Idle
    state = Idle;

    // Texture ban đầu
    sprite.setTexture(texIdle);
    sprite.setTextureRect(idleAnim.GetRect());

    // Đặt origin theo frame đầu tiên để nhân vật đứng vững
    auto rect = idleAnim.GetRect();
    sprite.setPosition(800, 300);
    sprite.setOrigin(384 / 2.f, 192 / 2.f);
    sprite.setScale(1.f, 1.f);

    // Các biến vật lý
    gravity = 900.f;
    jumpStrength = -450.f;
    isOnGround = true;
    groundY = 300.f; // y = vị trí mặt đất
    facingRight = true;
    wasMousePressedLastFrame = false;
}

void Player::HandleInput(float deltaTime)
{
    velocity.x = 0.f;
    bool moving = false;

    bool leftClick = sf::Keyboard::isKeyPressed(sf::Keyboard::X);

    // Chỉ cho di chuyển khi không tấn công
    if (state != Attacking1 && state != Attacking2 && state != Attacking3)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -250;
            facingRight = false;
            moving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = +250;
            facingRight = true;
            moving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && isOnGround)
        {
            velocity.y = jumpStrength;
            isOnGround = false;
        }
        // Flip sprite theo hướng
        sprite.setScale(facingRight ? 1.f : -1.f, 1.f);


        if (leftClick && !wasMousePressedLastFrame)
            ChangeState(Attacking1);
        else if (!leftClick)
            ChangeState(moving ? Walking : Idle);
        // Chỉ xử lý nhấn chuột 1 lần (chống spam)
        if (leftClick && !wasMousePressedLastFrame) {
            ChangeState(Attacking1);
        }
        else if (!leftClick) {
            ChangeState(moving ? Walking : Idle);
        }
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
    case Attacking1: attackAnim1.Reset();   break;
    case Attacking2: attackAnim2.Reset();   break;
    case Attacking3: attackAnim3.Reset();   break;
    }
}

// ----------------- Getter / Setter -----------------
sf::Sprite& Player::GetSprite() { return sprite; }
sf::Vector2f Player::GetPosition() const { return sprite.getPosition(); }
void Player::SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
