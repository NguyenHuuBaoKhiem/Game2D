#include "map.h"
#include <fstream>
#include <iostream>

MapLoader::MapLoader() : width(0), height(0) {}

bool MapLoader::loadFromFile(const std::string& ldtkFile) {
    std::ifstream file(ldtkFile);
    if (!file.is_open()) {
        std::cerr << "? Không th? m? file: " << ldtkFile << "\n";
        return false;
    }

    json data;
    file >> data;

    auto level = data["levels"][0];
    width = level["pxWid"];
    height = level["pxHei"];

    for (auto& layer : level["layerInstances"]) {
        if (!layer.contains("gridTiles")) continue;

        std::string tilesetPath = layer["__tilesetRelPath"];
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(tilesetPath)) {
            std::cerr << "? Không th? t?i tileset: " << tilesetPath << "\n";
            continue;
        }

        int tileSize = layer["__gridSize"];
        sf::Texture* texPtr = texture.get();

        for (auto& tile : layer["gridTiles"]) {
            auto px = tile["px"];
            auto src = tile["src"];

            sf::Sprite sprite(*texPtr);
            sprite.setTextureRect(sf::IntRect(src[0], src[1], tileSize, tileSize));
            sprite.setPosition(px[0], px[1]);
            tiles.push_back(sprite);
        }

        textures.push_back(std::move(texture));
    }

    std::cout << "? ?ã load map thành công!\n";
    return true;
}

void MapLoader::draw(sf::RenderWindow& window) {
    for (auto& s : tiles)
        window.draw(s);
}

void MapLoader::exportToImage(const std::string& filename) {
    sf::RenderTexture renderTex;
    renderTex.create(width, height);
    renderTex.clear(sf::Color::Transparent);

    for (auto& s : tiles)
        renderTex.draw(s);

    renderTex.display();
    sf::Image img = renderTex.getTexture().copyToImage();

    if (img.saveToFile(filename))
        std::cout << "?? ?ã l?u map ra: " << filename << "\n";
    else
        std::cerr << "? L?u ?nh th?t b?i!\n";
}
