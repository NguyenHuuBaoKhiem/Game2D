#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"

enum EnemyState {
    Idle,
    Walking,
    Attacking,
    TakeHit,
    Death
};

class Enemy {
private:
    // Âm thanh
    sf::SoundBuffer attackBuffer;
    sf::Sound attackSound;
    sf::SoundBuffer takeHitBuffer;
    sf::Sound takeHitSound;
    sf::SoundBuffer deathBuffer;
    sf::Sound deathSound;

    sf::Sprite sprite;
    sf::Vector2f velocity;

    bool facingRight = true; // Hướng mặt của enemy (true: phải, false: trái)

    EnemyState state;

    // Các animation
    Animation idleAnim;
    Animation walkAnim;
    Animation attackAnim;
    Animation takeHitAnim;
    Animation deathAnim;

    // Vật lý
    bool isOnGround = true;
    float gravity = 900.f; // Lực hấp dẫn (giống player)
    float groundY = 400.f; // Vị trí mặt đất (giống player)

    // Biến cho hành vi của enemy
    float moveSpeed = 100.f; // Tốc độ di chuyển
    float detectionRange = 300.f; // Phạm vi phát hiện player
    float attackRange = 50.f; // Phạm vi tấn công
    float attackCooldown = 1.0f; // Thời gian hồi chiêu tấn công
    float attackCooldownTimer = 0.f; // Timer để đếm thời gian hồi chiêu
    int health = 100; // Máu của enemy

    sf::FloatRect currentAttackBox; // Hitbox tấn công
    sf::FloatRect bodyHitbox; // Hitbox thân (để nhận sát thương)
    sf::Vector2f bodyOffset; // Offset để căn chỉnh hitbox thân

public:
    // Constructor
    Enemy(sf::Texture& texIdle, sf::Texture& texWalk, sf::Texture& texAttack,
        sf::Texture& texTakeHit, sf::Texture& texDeath);

    // Hàm xử lý logic
    void HandleInput(float deltaTime, const sf::Vector2f& playerPosition);
    void Update(float deltaTime);
    void Draw(sf::RenderWindow& window);

    // Quản lý trạng thái
    void ChangeState(EnemyState newState);

    // Nhận sát thương
    void TakeDamage(int damage);

    // Getter
    sf::Sprite& GetSprite();
    sf::Vector2f GetPosition() const;
    void SetPosition(const sf::Vector2f& pos);
    sf::FloatRect GetAttackBox() const { return currentAttackBox; }
    sf::FloatRect GetBodyHitbox() const { return bodyHitbox; }
    bool IsDead() const;
};
