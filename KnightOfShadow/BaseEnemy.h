#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>

// --- Trạng thái chung cho tất cả Enemy ---
enum class EnemyState {
    Idle,
    Walking,
    Attacking,
    Death
};

// --- Lớp cơ sở (BaseEnemy) ---
class BaseEnemy {
protected:
    sf::Sprite sprite;
    EnemyState state = EnemyState::Idle;
    bool facingRight = true;
    bool isDead = false;
    float health = 100.f;

    // --- Vật lý ---
    sf::Vector2f velocity;
    bool isOnGround = true;
    float gravity = 900.f;
    float groundY = 400.f;

    // --- Hitbox ---
    sf::FloatRect currentAttackBox;
    sf::FloatRect bodyHitbox;
    sf::Vector2f bodyOffset;

    // --- Bị Đánh ---
    bool recentlyHit = false;
    float hitCooldown = 0.4f;
    float hitTimer = 0.f;

    bool deathAnimFinished = false;

public:
    virtual ~BaseEnemy() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;

    virtual bool IsDeathAnimFinished() const { return deathAnimFinished; }

    virtual void TakeDamage(int damage) {
        if (isDead || recentlyHit) return;
        health -= damage;
        recentlyHit = true;
        std::cout << "Boss HP: " << health << "\n";
        if (health <= 0) {
            health = 0;
            isDead = true;
            state = EnemyState::Death;
        }
    }

    void UpdateHitCooldown(float dt) {
        if (recentlyHit) {
            hitTimer += dt;
            if (hitTimer >= hitCooldown) {
                recentlyHit = false;
                hitTimer = 0.f;
            }
        }
    }
    bool IsDead() const { return isDead; }
    EnemyState GetState() const { return state; }

    sf::Vector2f GetPosition() const { return sprite.getPosition(); }
    void SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
    sf::Sprite& GetSprite() { return sprite; }

    sf::FloatRect GetAttackBox() const { return currentAttackBox; }
    sf::FloatRect GetBodyHitbox() const { return bodyHitbox; }
};
