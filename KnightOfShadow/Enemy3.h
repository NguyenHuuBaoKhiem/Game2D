#pragma once
#include "BaseEnemy.h"
#include "BossAnimation.h"

enum class Boss3AttackType {
    None,
    Attack,
    Attack1,
    Attack2
};

class Enemy3 : public BaseEnemy {
private:
    // === Animation ===
    BossAnimation idleAnim;
    BossAnimation walkAnim;
    BossAnimation attackAnim;
    BossAnimation attack1Anim;
    BossAnimation attack2Anim;
    BossAnimation teleAnim;
    BossAnimation deathAnim;

    // === Tấn công ===
    Boss3AttackType attackType = Boss3AttackType::None;
    // --- Hành vi ---
    float moveSpeed = 200.f;
    float detectionRange = 800.f;
    float attackRange = 500.f;

    float attackCooldown = 0.5f;
    float attackCooldownTimer = 0.f;
    float attackTimer = 0.f;

    float attackActiveTime_Attack = 0.3f;
    float attackDuration_Attack = 0.7f;

    float attackActiveTime_Attack1 = 0.8f;  
    float attackDuration_Attack1 = 0.5f;

    float attackActiveTime_Attack2 = 0.5f;
    float attackDuration_Attack2 = 1.0f;


    // Teleport skill
    bool isTeleporting = false;  // đang teleport tổng thể
    bool isTeleOut = false;      // animation teleport-out
    bool isTeleIn = false;       // animation teleport-in
    float teleTimer = 0.f;       // timer của teleport
    float teleCooldown = 1.f;    // cooldown skill
    float teleDuration = 0.f;   // thời gian animation teleport nếu muốn
    sf::Vector2f teleTarget;     // vị trí sau teleport

    bool isTeleportForAttack2 = false;  // true khi boss chuẩn bị tele lên player để đánh Attack2
    sf::Vector2f attack2Target;         // vị trí teleport lên đầu player

    bool justFinishedAttack2 = false;             // bật true ngay sau khi Attack2 kết thúc
    float teleCooldownNormal = 6.f;             // cooldown teleport bình thường
    float teleCooldownAfterAttack2 = 0.2f;

    float groundY = 550.f;
    bool hitboxActive = false;

    // === Hàm nội bộ ===
    void UpdateAttackAnim(float deltaTime, const sf::Texture*& tex, sf::IntRect& rect);
    void ChangeState(EnemyState newState);

public:
    // === Constructor ===
    Enemy3(sf::Texture& texIdle,
        sf::Texture& texWalk,
        sf::Texture& texAttack,
        sf::Texture& texAttack1,
        sf::Texture& texAttack2,
        sf::Texture& texTele,
        sf::Texture& texDeath);

    // === Override ===
    void HandleInput(float deltaTime, const sf::Vector2f& playerPosition);
    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow& window) override;
    void TakeDamage(int damage) override;
};