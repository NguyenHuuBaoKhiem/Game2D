#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"
#include <vector>

struct DashTrail {
    sf::Sprite sprite;
    float alpha = 0.f;
};

extern std::vector<DashTrail> dashTrails;
extern float trailSpawnTimer;
extern float trailSpawnInterval;

enum PlayerState {
    Idle,
    Walking,
    Attacking1,
    Attacking2,
    Attacking3
};

class Player {
private:
    sf::SoundBuffer dashBuffer;
    sf::Sound dashSound;

    sf::SoundBuffer jumpBuffer;
    sf::Sound jumpSound;

    sf::SoundBuffer at1_buffer;
    sf::SoundBuffer at2_buffer;
    sf::SoundBuffer at3_buffer;
    sf::Sound attackSound;

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
    float gravity = 900.f;      // lực hấp dẫn
    float jumpStrength = -450.f; // lực nhảy (âm vì hướng lên trên)
    float groundY = 400.f; // y = vị trí mặt đất

    // Dash variables
    bool isDashing = false;
    float dashTime = 0.f;
    float dashDuration = 0.18f;   // thời gian dash (giây)
    float dashSpeed = 500.f;      // tốc độ dash
    float dashCooldown = 0.5f;    // thời gian hồi dash
    float dashCooldownTimer = 0.f;

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