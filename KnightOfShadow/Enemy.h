#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "BossAnimation.h"

enum class EnemyState {
    Idle,
    Walking,
    Attacking,
    TakeHit,
    Death
};

class Enemy {
private:
    // --- Âm thanh ---
    sf::SoundBuffer attackBuffer;
    sf::Sound attackSound;
    sf::SoundBuffer takeHitBuffer;
    sf::Sound takeHitSound;
    sf::SoundBuffer deathBuffer;
    sf::Sound deathSound;

    // --- Sprite & trạng thái ---
    sf::Sprite sprite;
    sf::Vector2f velocity;
    bool facingRight = true;
    EnemyState state;

    // --- Animation (Boss) ---
    BossAnimation idleAnim;
    BossAnimation walkAnim;
    BossAnimation attackAnim;
    BossAnimation takeHitAnim;
    BossAnimation deathAnim;

    // --- Vật lý ---
    bool isOnGround = true;
    float gravity = 900.f;
    float groundY = 400.f;

    // --- Hành vi ---
    float moveSpeed = 100.f;
    float detectionRange = 900.f;
    float attackRange = 200.f;
    float attackCooldown = 1.0f;
    float attackCooldownTimer = 0.f;
    int health = 10000000;
    float attackActiveTime = 0.65f;  // Delay trước khi kích hoạt hitbox
    float attackDuration = 0.7f;     // Thời gian hitbox tồn tại
    bool hitboxActive = false;
    float attackTimer = 0.f;

    // --- Hitbox ---
    sf::FloatRect currentAttackBox;
    sf::FloatRect bodyHitbox;
    sf::Vector2f bodyOffset;
    sf::FloatRect detectionArea;

public:
    // --- Constructor ---
    Enemy(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack,
        sf::Texture& texTakeHit, sf::Texture& texDeath);

    // --- Hành vi ---
    void HandleInput(float deltaTime, const sf::Vector2f& playerPosition);
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);

    // --- Quản lý trạng thái ---
    void ChangeState(EnemyState newState);
    void TakeDamage(int damage);

    // --- Getter / Setter ---
    sf::Sprite& GetSprite();
    sf::Vector2f GetPosition() const;
    void SetPosition(const sf::Vector2f& pos);
    sf::FloatRect GetAttackBox() const { return currentAttackBox; }
    sf::FloatRect GetBodyHitbox() const { return bodyHitbox; }
    bool IsDead() const;
};
