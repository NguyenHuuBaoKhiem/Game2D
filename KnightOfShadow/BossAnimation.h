#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class BossAnimation {
private:
    sf::Texture* texture; // Texture của sprite sheet
    std::vector<sf::IntRect> frames; // Danh sách các frame
    float frameTime; // Thời gian mỗi frame (giây)
    float currentTime; // Thời gian đã trôi qua
    int currentFrame; // Frame hiện tại
    bool isLooping; // Animation có lặp lại không
    bool isFinished; // Animation đã kết thúc (dùng cho animation không lặp)

public:
    // Constructor
    BossAnimation(sf::Texture& tex, sf::Vector2i frameCount, float switchTime, bool looping = true);

    // Cập nhật animation
    void Update(float deltaTime);

    // Reset animation về frame đầu
    void Reset();

    // Getter
    const sf::Texture* GetTexture() const { return texture; }
    sf::IntRect GetRect() const { return frames[currentFrame]; }
    bool IsFinished() const { return isFinished; }
};
