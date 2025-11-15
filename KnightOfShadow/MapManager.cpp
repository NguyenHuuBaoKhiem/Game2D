#include "MapManager.h"
#include <iostream>

void MapManager::Init() {
    levelNames = { "Level_0", "Level_1", "Level_2", "Level_3" };
    LoadLevel(0);
}

void MapManager::LoadLevel(int index) {
    currentLevel = index;
    map.Load("Assets/Images/Map/Map.ldtk", levelNames[currentLevel]);

    // Dừng nhạc cũ
    if (bgMusic.getStatus() == sf::Music::Playing)
        bgMusic.stop();

    std::string musicPath;

    // Nhạc nền và trạng thái "được đi tiếp"
    switch (currentLevel) {
    case 0:
        musicPath = "Assets/Sound effect/Music/intro.ogg";
        canGoNext = true; // map 0 đi thẳng qua luôn
        break;
    case 1:
        musicPath = "Assets/Sound effect/Music/boss1.ogg";
        canGoNext = false;
        break;
    case 2:
        musicPath = "Assets/Sound effect/Music/boss2.ogg";
        canGoNext = false;
        break;
    case 3:
        musicPath = "Assets/Sound effect/Music/final.ogg";
        canGoNext = false;
        break;
    }

    if (!musicPath.empty() && bgMusic.openFromFile(musicPath)) {
        bgMusic.setLoop(true);
        bgMusic.setVolume(60.f);
        bgMusic.play();
    }

}

void MapManager::Update(float deltaTime) {
    // Nếu cần hiệu ứng nhạc hay logic theo map, thêm ở đây
}

void MapManager::Draw(sf::RenderWindow& window) {
    map.Draw(window);
}
