#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

enum PlayerState {
    Idle,
    Walking,
    Attacking1,
    Attacking2,
    Attacking3
};

class Player {
private:
    sf::Sprite sprite;
    sf::Vector2f velocity;

    bool facingRight = true;
    bool comboQueued = false;
    bool wasMousePressedLastFrame = false; // để chống spam click combo

    PlayerState state;

    // Các animation
    Animation idleAnim;
    Animation walkAnim;
    Animation attackAnim1;
    Animation attackAnim2;
    Animation attackAnim3;

    bool isOnGround = true;
    float gravity = 600.f;      // lực hấp dẫn
    float jumpStrength = -350.f; // lực nhảy (âm vì hướng lên trên)
    float groundY = 500.f;

public:
    // Constructor đồng bộ với Player.cpp
    Player(sf::Texture& texIdle, sf::Texture& texWalk,
        sf::Texture& texAttack1, sf::Texture& texAttack2, sf::Texture& texAttack3);

    void HandleInput(float deltaTime);
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);

    // Quản lý trạng thái
    void ChangeState(PlayerState newState);

    // Hỗ trợ vị trí và sprite
    sf::Sprite& GetSprite();
    sf::Vector2f GetPosition() const;
    void SetPosition(const sf::Vector2f& pos);
};
