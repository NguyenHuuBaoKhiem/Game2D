#include "MapManager.h"
#include <iostream>

void MapManager::Init() {
    // Danh sách tên map
    levelNames = { "Level_0", "Level_1", "Level_2", "Level_3" };
    LoadLevel(0); // load map đầu tiên
}

void MapManager::LoadLevel(int index) {
    currentLevel = index;
    map.Load("Assets/Images/Map/Map.ldtk", levelNames[currentLevel]);

    // Dừng nhạc cũ nếu đang phát
    if (bgMusic.getStatus() == sf::Music::Playing)
        bgMusic.stop();

    std::string musicPath;
    float volume = 100.f; // âm lượng mặc định

    // Chọn nhạc và âm lượng cho từng map
    switch (currentLevel) {
    case 0:
        musicPath = "Assets/Music/intro.ogg";
        canGoNext = true;  // map intro đi thẳng
        volume = 40.f;
        break;
    case 1:
        musicPath = "Assets/Music/boss2.mp3";
        canGoNext = false;
        volume = 70.f;
        break;
    case 2:
        musicPath = "Assets/Music/boss1.mp3";
        canGoNext = false;
        volume = 80.f;
        break;
    case 3:
        musicPath = "Assets/Music/boss3.mp3";
        canGoNext = false;
        volume = 80.f;
        break;
    default:
        canGoNext = false;
        break;
    }

    // Mở nhạc nếu có đường dẫn
    if (!musicPath.empty() && bgMusic.openFromFile(musicPath)) {
        bgMusic.setLoop(true);       // lặp nhạc liên tục
        bgMusic.setVolume(volume);   // âm lượng theo map
        bgMusic.play();              // bắt đầu phát nhạc
    }
}

void MapManager::Update(float deltaTime) {
    // Nếu muốn hiệu ứng nhạc theo map, fade in/out hoặc logic khác, thêm ở đây
}

void MapManager::Draw(sf::RenderWindow& window) {
    map.Draw(window); // vẽ map
}
