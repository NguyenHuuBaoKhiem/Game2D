#pragma once
#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <string>
#include <vector>

using json = nlohmann::json;

class MapLoader {
private:
    std::vector<sf::Sprite> tiles;
    std::vector<std::unique_ptr<sf::Texture>> textures;
    int width, height;

public:
    MapLoader();

    bool loadFromFile(const std::string& ldtkFile);
    void draw(sf::RenderWindow& window);
    void exportToImage(const std::string& filename);
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif
