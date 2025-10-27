#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include "Animation.h"
#include <vector>

struct EnemyTrail {
    sf::Sprite sprite;
    float alpha = 0.f;

extern std::vector<EnemyTrail> enemyTrails;
extern float trailSpawnTimer;
extern float trailSpawnInterval;

enum EnemyState {
    Idle,
    Walk,
    Attack,
    TakeHit,
    Death
};

class Enemy {
private:
    sf::Sprite sprite;
    Animation* currentAnim = nullptr;

    Animation idleAnim;
    Animation walkAnim;
    Animation attackAnim;
    Animation takeHitAnim;
    Animation deathAnim;

    sf::SoundBuffer attackBuffer;
    sf::Sound attackSound;
    sf::SoundBuffer hitBuffer;
    sf::Sound hitSound;
    sf::SoundBuffer deathBuffer;
    sf::Sound deathSound;

    int hp = 500;
    int maxHp = 500;
    int damage = 20;
    float speed = 150.f;
    float attackCooldown = 0.f;
    float hitTimer = 0.f;
    float detectionRange = 300.f;
    float attackRange = 50.f;
    bool facingLeft = false;

    sf::Vector2u screenSize;
    sf::FloatRect hitbox;

public:
    Enemy(float x, float y, sf::Vector2u screenSize);
    ~Enemy();
    void Update(float deltaTime, const sf::Vector2f& playerPos);
    void Draw(sf::RenderWindow& window);
    void TakeDamage(int amount);

    // Getter
    sf::Vector2f GetPosition() const { return sprite.getPosition(); }
    sf::FloatRect GetHitbox() const { return hitbox; } 
    int GetHP() const { return hp; }
    bool IsDead() const { return hp <= 0 && state == Death; }
private:
    EnemyState state = Idle;
    void ChangeState(EnemyState newState);
    void updateHitbox();
    float getDistanceToPlayer(const sf::Vector2f& playerPos);
    void moveTowardsPlayer(float dt, const sf::Vector2f& playerPos);
    void performAttack();
    void drawHPBar(sf::RenderTarget& target, sf::RenderStates states) const;
};
