#pragma once
#include "BaseEnemy.h"
#include "BossAnimation.h"

class Enemy1 : public BaseEnemy {
private:
    // --- Animation ---
    BossAnimation idleAnim;
    BossAnimation walkAnim;
    BossAnimation attackAnim;
    BossAnimation deathAnim;


    // --- Hành vi ---
    float moveSpeed = 100.f;
    float detectionRange = 700.f;
    float attackRange = 200.f;
    float attackCooldown = 1.0f;
    float attackCooldownTimer = 0.f;
    float attackActiveTime = 0.65f;
    float attackDuration = 0.7f;
    bool hitboxActive = false;
    float attackTimer = 0.f;

  

public:
    // --- Constructor ---
    Enemy1(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack, sf::Texture& texDeath);

    // --- Giữ nguyên hành vi ---
    void HandleInput(float deltaTime, const sf::Vector2f& playerPosition);
    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;
    void ChangeState(EnemyState newState);
    void TakeDamage(int damage) override; // override từ BaseEnemy

};
