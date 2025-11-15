#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

// --- Trạng thái Enemy ---
enum class EnemyState {
    Idle,
    Walking,
    Attacking,
    Death
};

// --- Lớp cơ sở Enemy ---
class BaseEnemy {
protected:
    sf::Sprite sprite;
    EnemyState state = EnemyState::Idle;
    bool facingRight = true;
    bool isDead = false;

    // --- Máu ---
    float health = 100.f;
    float maxHealth = 100.f;

    // --- Vật lý ---
    sf::Vector2f velocity;
    bool isOnGround = true;
    float gravity = 900.f;
    float groundY = 400.f;

    // --- Hitbox ---
    sf::FloatRect currentAttackBox;
    sf::FloatRect bodyHitbox;
    sf::Vector2f bodyOffset;

    // --- Cooldown bị đánh ---
    bool recentlyHit = false;
    float hitCooldown = 0.4f;
    float hitTimer = 0.f;

    bool deathAnimFinished = false;

public:
    virtual ~BaseEnemy() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;

    // === Lấy máu ===
    float GetHealth() const { return health; }
    float GetMaxHealth() const { return maxHealth; }

    // === Enemy là Boss? ===
    virtual bool IsBoss() const { return false; }

    // === Nhận sát thương ===
    virtual void TakeDamage(int damage) {
        if (isDead || recentlyHit) return;

        health -= damage;
        if (health < 0) health = 0;

        std::cout << "Boss health: " << health << "\n";

        recentlyHit = true;

        if (health <= 0) {
            isDead = true;
            state = EnemyState::Death;
        }
    }

    // === Cooldown bị đánh ===
    void UpdateHitCooldown(float dt) {
        if (!recentlyHit) return;
        hitTimer += dt;
        if (hitTimer >= hitCooldown) {
            recentlyHit = false;
            hitTimer = 0.f;
        }
    }

    // === Getter cơ bản ===
    bool IsDead() const { return isDead; }
    EnemyState GetState() const { return state; }

    sf::Vector2f GetPosition() const { return sprite.getPosition(); }
    void SetPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
    sf::Sprite& GetSprite() { return sprite; }

    bool IsAttacking() const { return state == EnemyState::Attacking; }

    sf::FloatRect GetAttackBox() const { return currentAttackBox; }
    sf::FloatRect GetBodyHitbox() const { return bodyHitbox; }

    // Safe zone (Boss 3 cần)
    virtual std::vector<sf::FloatRect> GetSafeZones() const { return {}; }

    bool IsDeathAnimFinished() const { return deathAnimFinished; }
};