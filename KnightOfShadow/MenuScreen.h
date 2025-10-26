#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class MenuScreen {
private:
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    sf::Font font;
    sf::Font titleFont;
    sf::Text titleText;
    sf::Text pressText;
    sf::RectangleShape fadeRect;

    float fadeAlpha = 255.f;
    float blinkTimer = 0.f;
    float blinkSpeed = 2.f;
    bool showText = true;

    float fadeOutAlpha = 0.f;

    sf::SoundBuffer introBuffer;
    sf::Sound introSound;
    sf::SoundBuffer startBuffer;
    sf::Sound startSound;
    sf::Music bgMusic;
    float musicVolume = 60.f;


public:
    bool isActive = true;
    bool isFadingOut = false;

    MenuScreen(sf::RenderWindow& window);
    void Update(float dt, sf::RenderWindow& window);
    void Draw(sf::RenderWindow& window);
};
